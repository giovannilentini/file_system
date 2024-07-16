#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>

#include "../include/file_system.h"

void *fs;
int *FAT;
char *data_blocks;
int current_dir_index = 0;

/* ===== HANDLE FUNCTION ===== */

char* expand_path(const char* path) {
    if (path[0] == '~') {
        const char* home = getenv("HOME");
        if (!home) {
            struct passwd* pw = getpwuid(getuid());
            home = pw->pw_dir;
        }
        char* expanded_path = malloc(strlen(home) + strlen(path));
        if (!expanded_path) {
            fprintf(stderr, "Error: memory allocation failed\n");
            exit(1);
        }
        strcpy(expanded_path, home);
        strcat(expanded_path, path + 1);
        return expanded_path;
    } else {
        return strdup(path);
    }
}

void print_all_entries() {
    printf("Listing all non-empty entries:\n");
    for (int block = 0; block < MAX_BLOCKS; block++) {
        if (FAT[block] != FAT_FREE) {
            for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
                FileEntry *entry = (FileEntry *)(data_blocks + block * BLOCK_SIZE + i * sizeof(FileEntry));
                if (entry->name[0] != '\0') {
                    printf("Block %d, Entry %d: %s (%s)\n", block, i, entry->name, entry->is_directory ? "Directory" : "File");
                }
            }
        }
    }
    printf("End of listing.\n");
}

/*
    Syncronize the changes in the disk image.
*/
void sync_fs() {
    msync(fs, FILE_SIS_SIZE, MS_SYNC);
}

/*
    Allocates a free block in the FAT and marks it as EOF.
    Return the index of the allocated block, or -1 if no free 
    block is found.
*/
int allocate_block() {
    for (int i = 1; i < MAX_BLOCKS; i++) {
        if (FAT[i] == FAT_FREE) {
            FAT[i] = MY_EOF;
            return i;
        }
    }
    return -1;
}

int find_file_entry(const char *name, int dir_block) {
    int block = dir_block;

    while (block != MY_EOF) {
        for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
            FileEntry *entry = (FileEntry *)(data_blocks + block * BLOCK_SIZE + i * sizeof(FileEntry));

            if (entry->first_block != FAT_FREE && strcmp(entry->name, name) == 0) {
                return block * (BLOCK_SIZE / sizeof(FileEntry)) + i;
            }
        }

        block = FAT[block];
    }

    printf("Error: File or Directory '%s' not found in the current directory.\n", name);
    return -1;
}

int is_empty(char* dirname) {
    int file_entry_index = find_file_entry(dirname, current_dir_index);
    if (file_entry_index == -1) {
        return -1;
    }

    FileEntry *file = (FileEntry *)(
        data_blocks +                             // Start of the memory region
        (file_entry_index / (BLOCK_SIZE / sizeof(FileEntry))) * BLOCK_SIZE +  // Offset to the start of the block
        (file_entry_index % (BLOCK_SIZE / sizeof(FileEntry))) * sizeof(FileEntry) // Offset within the block
    );

    int dir_block = file->first_block;
    int is_empty = 1;

    while (dir_block != MY_EOF && is_empty) {
        for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
            FileEntry *entry = (FileEntry *)(data_blocks + dir_block * BLOCK_SIZE + i * sizeof(FileEntry));
            if (entry->name[0] != '\0') {
                is_empty = 0;
                break;
            }
        }
        dir_block = FAT[dir_block];
    }

    return is_empty;
}

const char* get_current_dir_name() {
    for (int block = 0; block < MAX_BLOCKS; block++) {
        if (FAT[block] != FAT_FREE) {
            for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
                FileEntry *entry = (FileEntry *)(data_blocks + block * BLOCK_SIZE + i * sizeof(FileEntry));

                if (entry->name[0] != '\0' && entry->is_directory && entry->first_block == current_dir_index) {
                    return entry->name;
                }
            }
        }
    }

    return NULL;
}

FileEntry* open_file_entry(const char* name) {
    int current_block = current_dir_index;

    while (current_block != MY_EOF) {
        FileEntry* current_dir = (FileEntry*)(data_blocks + current_block * BLOCK_SIZE);

        for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
            FileEntry* entry = &current_dir[i];

            if (entry->name[0] != '\0' && strcmp(entry->name, name) == 0) {
                return entry;
            }
        }

        current_block = FAT[current_block];
    }

    printf("Error: File or Directory '%s' not found in the current directory.\n", name);
    return NULL;
}

/* ===== DISK FUNCTION ===== */

void init(const char *name) {
    int fd = open(name, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        printf("Error: Cannot open the file in the init function\n");
        exit(1);
    }

    ftruncate(fd, FILE_SIS_SIZE);

    fs = mmap(NULL, FILE_SIS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fs == MAP_FAILED) {
        printf("Error: Cannot map the file in the init function\n");
        close(fd);
        exit(1);
    }
    close(fd);

    FAT = (int*) fs;
    data_blocks = (char*) (FAT + MAX_BLOCKS);

    if (FAT[0] == FAT_FREE) {
        FAT[0] = MY_EOF;
        
        FileEntry root;
        strcpy(root.name, "/");
        root.first_block = 0;
        root.size = 0;
        root.is_directory = 1;
        root.current_position = 0;

        memcpy(data_blocks, &root, sizeof(FileEntry));

        sync_fs();
    }

    current_dir_index = 0;
}

void erase_disk() {
    memset(FAT, FAT_FREE, FILE_SIS_SIZE / BLOCK_SIZE * sizeof(int));
    memset(data_blocks, 0, FILE_SIS_SIZE - (sizeof(int) * MAX_BLOCKS));

    FAT[0] = MY_EOF;

    FileEntry root;
    strcpy(root.name, "/");
    root.first_block = 0;
    root.size = 0;
    root.is_directory = 1;
    root.current_position = 0;

    memcpy(data_blocks, &root, sizeof(FileEntry));

    current_dir_index = 0;

    sync_fs();
}

/* ===== FILE FUNCTION ===== */

int create_file(const char *filename) {
    int current_block = current_dir_index;

    while (current_block != MY_EOF) {
        FileEntry *current_dir = (FileEntry *)(data_blocks + current_block * BLOCK_SIZE);
        for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
            FileEntry *entry = current_dir + i;
            if (strcmp(entry->name, filename) == 0) {
                printf("Error: File or Directory with the name '%s' already exists.\n", filename);
                return -1;
            }
        }
        current_block = FAT[current_block];
    }

    current_block = current_dir_index;
    int last_block = current_dir_index;

    while (current_block != MY_EOF) {
        FileEntry *current_dir = (FileEntry *)(data_blocks + current_block * BLOCK_SIZE);

        for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
            FileEntry *entry = current_dir + i;
            if (entry->name[0] == '\0') {
                strcpy(entry->name, filename);
                entry->is_directory = 0;
                entry->first_block = allocate_block();
                entry->current_position = 0;
                if (entry->first_block == -1) {
                    printf("Error: No free blocks available.\n");
                    return -1;
                }

                sync_fs();
                return 0;
            }
        }
        last_block = current_block;
        current_block = FAT[current_block];
    }

    int new_block = allocate_block();
    if (new_block == -1) {
        printf("Error: No free blocks available.\n");
        return -1;
    }
    
    FAT[last_block] = new_block;
    FAT[new_block] = MY_EOF;

    FileEntry *new_file = (FileEntry *)(data_blocks + new_block * BLOCK_SIZE);
    strcpy(new_file->name, filename);
    new_file->is_directory = 0;
    new_file->first_block = allocate_block();
    new_file->current_position = 0;
    if (new_file->first_block == -1) {
        printf("Error: No free blocks available.\n");
        return -1;
    }

    sync_fs();
    return 0;
}

int write_file(const char *filename, const char *buffer, int size) {
    int file_entry_index = find_file_entry(filename, current_dir_index);
    if (file_entry_index == -1) {
        return -1;
    }

    FileEntry *file = (FileEntry *)(
        data_blocks +                             // Start of the memory region
        (file_entry_index / (BLOCK_SIZE / sizeof(FileEntry))) * BLOCK_SIZE +  // Offset to the start of the block
        (file_entry_index % (BLOCK_SIZE / sizeof(FileEntry))) * sizeof(FileEntry) // Offset within the block
    );

    if (file->is_directory) {
        printf("Error: cannot write a directory.\n");
        return -1;
    }

    file->size = file->current_position + size > file->size ? file->current_position + size : file->size;
    int current_block = file->first_block;
    int remaining_size = size;
    int offset = 0;
    int current_position = file->current_position;

    while (current_position >= BLOCK_SIZE) {
        if (FAT[current_block] == MY_EOF) {
            int new_block = allocate_block();
            if (new_block == -1) {
                printf("No more blocks available.\n");
                return -1;
            }
            FAT[current_block] = new_block;
            FAT[new_block] = MY_EOF;
        }
        current_block = FAT[current_block];
        current_position -= BLOCK_SIZE;
    }

    while (remaining_size > 0) {
        // Calculate the position within the current block where we need to start writing
        int pos_in_block = current_position % BLOCK_SIZE;
        // Determine the amount of data to write in the current block
        int to_write = (remaining_size < (BLOCK_SIZE - pos_in_block)) ? remaining_size : (BLOCK_SIZE - pos_in_block);

        // Copy data from the buffer to the correct position in the data blocks
        memcpy(data_blocks + current_block * BLOCK_SIZE + pos_in_block, buffer + offset, to_write);

        // Update the remaining size, buffer offset, and current position in the file
        remaining_size -= to_write;
        offset += to_write;
        current_position += to_write;

        // Check if there is more data to write and if we need a new block
        if (remaining_size > 0) {
            // If the current block is the last block allocate a new one
            if (FAT[current_block] == MY_EOF) {
                int new_block = allocate_block();
                if (new_block == -1) {
                    printf("No more blocks available.\n");
                    return -1;
                }
                // Link the current block to the new block in the FAT
                FAT[current_block] = new_block;
                FAT[new_block] = MY_EOF;
            }
            // Move to the next block and reset the position to the start of the block
            current_block = FAT[current_block];
            current_position = 0;
        }
    }

    sync_fs();
    return size;
}

int read_file(const char *filename, char *buffer, int size) {
    int file_entry_index = find_file_entry(filename, current_dir_index);
    if (file_entry_index == -1) {
        return -1;
    }

    FileEntry *file = (FileEntry *)(
        data_blocks +                             // Start of the memory region
        (file_entry_index / (BLOCK_SIZE / sizeof(FileEntry))) * BLOCK_SIZE +  // Offset to the start of the block
        (file_entry_index % (BLOCK_SIZE / sizeof(FileEntry))) * sizeof(FileEntry) // Offset within the block
    );

    if (file->is_directory) {
        printf("Error: cannot read a directory.\n");
        return -1;
    }

    int current_block = file->first_block;
    int remaining_size = file->size;
    int offset = 0;
    int current_position = file->current_position;

    while (current_position >= BLOCK_SIZE) {
        if (FAT[current_block] == MY_EOF) {
            printf("Reached EOF while seeking to current position.\n");
            return -1;
        }
        current_block = FAT[current_block];
        current_position -= BLOCK_SIZE;
    }

    // Read data starting from the current_position
    while (remaining_size > 0 && current_block != MY_EOF) {
        int pos_in_block = current_position % BLOCK_SIZE;
        int to_read = (remaining_size < (BLOCK_SIZE - pos_in_block)) ? remaining_size : (BLOCK_SIZE - pos_in_block);

        // Copy the data from the file system's data blocks to the buffer
        memcpy(buffer + offset, data_blocks + current_block * BLOCK_SIZE + pos_in_block, to_read);

        remaining_size -= to_read;
        offset += to_read;
        current_position += to_read;

        // Move to the next block if there's more data to read
        if (remaining_size > 0) {
            current_block = FAT[current_block];
            current_position = 0;
        }
    }

    sync_fs();
    return offset;
}

int seek(const char *filename, int position) {
    int file_entry_index = find_file_entry(filename, current_dir_index);
    if (file_entry_index == -1) {
        return -1;
    }

    FileEntry *file = (FileEntry *)(
        data_blocks +                             // Start of the memory region
        (file_entry_index / (BLOCK_SIZE / sizeof(FileEntry))) * BLOCK_SIZE +  // Offset to the start of the block
        (file_entry_index % (BLOCK_SIZE / sizeof(FileEntry))) * sizeof(FileEntry) // Offset within the block
    );
    
    if (file->is_directory) {
        printf("Error: cannot seek a directory.\n");
        return -1;
    }

    if (position > file->size) {
        printf("Error: seek position is beyond the file size.\n");
        return -1;
    }

    file->current_position = position;
    sync_fs();
    return 0;
}

int erase_file(const char *filename) {
    int file_entry_index = find_file_entry(filename, current_dir_index);
    if (file_entry_index == -1) {
        return -1;
    }

    FileEntry *file = (FileEntry *)(
        data_blocks +                             // Start of the memory region
        (file_entry_index / (BLOCK_SIZE / sizeof(FileEntry))) * BLOCK_SIZE +  // Offset to the start of the block
        (file_entry_index % (BLOCK_SIZE / sizeof(FileEntry))) * sizeof(FileEntry) // Offset within the block
    );

    if (file->is_directory && !is_empty(file->name)) {
        printf("Error: Directory '%s' is not empty.\n", filename);
        return -1;
    }

    int current_block = file->first_block;
    while (current_block != MY_EOF) {
        int next_block = FAT[current_block];
        FAT[current_block] = FAT_FREE;
        current_block = next_block;
    }

    memset(file, 0, sizeof(FileEntry));
    sync_fs();
    return 0;
}

/* ===== DIRECTORY FUNCTION ===== */

int create_dir(const char *dirname) {
    int current_block = current_dir_index;

    while (current_block != MY_EOF) {
        FileEntry *current_dir = (FileEntry *)(data_blocks + current_block * BLOCK_SIZE);
        for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
            FileEntry *entry = current_dir + i;
            if (strcmp(entry->name, dirname) == 0) {
                printf("Error: File or Directory with the name '%s' already exists.\n", dirname);
                return -1;
            }
        }
        current_block = FAT[current_block];
    }

    current_block = current_dir_index;
    int last_block = current_dir_index;

    while (current_block != MY_EOF) {
        FileEntry *current_dir = (FileEntry *)(data_blocks + current_block * BLOCK_SIZE);

        for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
            FileEntry *entry = current_dir + i;
            if (entry->name[0] == '\0') {
                strcpy(entry->name, dirname);
                entry->is_directory = 1;
                entry->first_block = allocate_block();
                if (entry->first_block == -1) {
                    printf("Error: No free blocks available.\n");
                    return -1;
                }

                sync_fs();
                return 0;
            }
        }

        last_block = current_block;
        current_block = FAT[current_block];
    }

    int new_block = allocate_block();
    if (new_block == -1) {
        printf("Error: No free blocks available.\n");
        return -1;
    }

    FAT[last_block] = new_block;
    FAT[new_block] = MY_EOF;

    FileEntry *new_dir = (FileEntry *)(data_blocks + new_block * BLOCK_SIZE);
    strcpy(new_dir->name, dirname);
    new_dir->is_directory = 1;
    new_dir->first_block = allocate_block();
    if (new_dir->first_block == -1) {
        printf("Error: No free blocks available.\n");
        return -1;
    }

    sync_fs();
    return 0;
}

int erase_dir_recursive(const char *dirname) {
    int dir_entry_index = find_file_entry(dirname, current_dir_index);
    if (dir_entry_index == -1) {
        return -1;
    }

    FileEntry *dir = (FileEntry *)(
        data_blocks +                             // Start of the memory region
        (dir_entry_index / (BLOCK_SIZE / sizeof(FileEntry))) * BLOCK_SIZE +  // Offset to the start of the block
        (dir_entry_index % (BLOCK_SIZE / sizeof(FileEntry))) * sizeof(FileEntry) // Offset within the block
    );

    if (!dir->is_directory) {
        printf("Error: '%s' is not a directory.\n", dirname);
        return -1;
    }

    int current_block = dir->first_block;

    int temp_dir_index = current_dir_index;
    current_dir_index = dir->first_block;

    while (current_block != MY_EOF) {
        FileEntry *current_dir = (FileEntry *)(data_blocks + current_block * BLOCK_SIZE);
        for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
            FileEntry *entry = current_dir + i;
            if (entry->name[0] == '\0') {
                continue;
            }

            int res;
            if (entry->is_directory) {
                res = erase_dir_recursive(entry->name);
                if (res == -1) return -1;
            } else {
                res = erase_file(entry->name);
                if (res == -1) return -1;
            }
        }
        current_block = FAT[current_block];
    }

    current_dir_index = temp_dir_index;

    erase_file(dirname);
    sync_fs();
    return 0;
}

int change_dir(const char *dirname) {
    if (strcmp(dirname, "..") == 0) {
        if (current_dir_index == 0) {
            printf("Already at root directory.\n");
            return -1;
        } else {
            for (int i = 0; i < MAX_BLOCKS; i++) {
                int current_block = i;
                while (current_block != MY_EOF) {
                    FileEntry *entry = (FileEntry *)(data_blocks + current_block * BLOCK_SIZE);
                    for (int j = 0; j < BLOCK_SIZE / sizeof(FileEntry); j++) {
                        FileEntry *sub_entry = entry + j;
                        if (sub_entry->is_directory && sub_entry->first_block == current_dir_index) {
                            current_dir_index = i;
                            return 0;
                        }
                    }

                    current_block = FAT[current_block];
                }
            }
            printf("Error: Parent directory not found.\n");
            return -1;
        }
    } else {
        int current_block = current_dir_index;

        while (current_block != MY_EOF) {
            FileEntry *current_dir = (FileEntry *)(data_blocks + current_block * BLOCK_SIZE);

            for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
                FileEntry *entry = current_dir + i;
                if (entry->name[0] != '\0' && entry->is_directory && strcmp(entry->name, dirname) == 0) {
                    current_dir_index = entry->first_block;
                    return 0;
                }
            }

            current_block = FAT[current_block];
        }

        printf("Error: Directory '%s' not found in current directory.\n", dirname);
        return -1;
    }
}

void ls_dir() {
    printf("Listing directory contents for directory '%s':\n", get_current_dir_name());

    int current_block = current_dir_index;

    while (current_block != MY_EOF) {
        FileEntry *current_dir = (FileEntry *)(data_blocks + current_block * BLOCK_SIZE);

        for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
            FileEntry *entry = current_dir + i;
            if (entry->name[0] != '\0' && strcmp(entry->name, "/") != 0 && entry->first_block != 0) {
                printf("%s: %s\n", entry->is_directory ? "Dir" : "File", entry->name);
            }
        }

        current_block = FAT[current_block];
    }

}

/* ===== COPY FUNCTION ===== */

int copy_to_my_fs(const char *source_filepath, const char *destination_filename) {
    char* expanded_source_filepath = expand_path(source_filepath);
    
    int file_entry_index = find_file_entry(destination_filename, current_dir_index);
    if (file_entry_index == -1) {
        free(expanded_source_filepath);
        return -1;
    }

    FileEntry *file = (FileEntry *)(
        data_blocks +                             // Start of the memory region
        (file_entry_index / (BLOCK_SIZE / sizeof(FileEntry))) * BLOCK_SIZE +  // Offset to the start of the block
        (file_entry_index % (BLOCK_SIZE / sizeof(FileEntry))) * sizeof(FileEntry) // Offset within the block
    );

    int fd = open(expanded_source_filepath, O_RDONLY);
    if (fd == -1) {
        printf("Error: File not found in the main file system\n");
        free(expanded_source_filepath);
        return -1;
    }

    struct stat file_stat;
    if (stat(expanded_source_filepath, &file_stat) == -1) {
        perror("Error stat in copy_to_my_fs");
        close(fd);
        free(expanded_source_filepath);
        return -1;
    }

    int file_size = file_stat.st_size;
    char *buf = malloc(file_size);
    if (buf == NULL) {
        perror("Error allocating buffer in copy_to_my_fs");
        close(fd);
        free(expanded_source_filepath);
        return -1;
    }

    if (read(fd, buf, file_size) == -1) {
        perror("Error read in copy_to_my_fs");
        close(fd);
        free(buf);
        free(expanded_source_filepath);
        return -1;
    }
    close(fd);

    seek(destination_filename, 0);
    write_file(destination_filename, buf, file_size);
    file->size = file_size;

    free(buf);
    free(expanded_source_filepath);

    return 0;
}

int copy_from_my_fs(const char *source_filename, const char *destination_filepath) {
    char* expanded_destination_filepath = expand_path(destination_filepath);

    int file_entry_index = find_file_entry(source_filename, current_dir_index);
    if (file_entry_index == -1) {
        free(expanded_destination_filepath);
        return -1;
    }

    FileEntry *file = (FileEntry *)(
        data_blocks +                             // Start of the memory region
        (file_entry_index / (BLOCK_SIZE / sizeof(FileEntry))) * BLOCK_SIZE +  // Offset to the start of the block
        (file_entry_index % (BLOCK_SIZE / sizeof(FileEntry))) * sizeof(FileEntry) // Offset within the block
    );

    int fd = open(expanded_destination_filepath, O_WRONLY | O_TRUNC | O_CREAT, 0666);
    if (fd == -1) {
        printf("Error: File not found in the main file system\n");
        free(expanded_destination_filepath);
        return -1;
    }

    int file_size = file->size;
    char *buf = malloc(file_size);
    if (buf == NULL) {
        perror("Error allocating buffer in copy_from_my_fs");
        close(fd);
        free(expanded_destination_filepath);
        return -1;
    }

    seek(source_filename, 0);
    read_file(source_filename, buf, file_size);

    if (write(fd, buf, file_size) == -1) {
        perror("Error write in copy_from_my_fs");
        close(fd);
        free(buf);
        free(expanded_destination_filepath);
        return -1;
    }

    close(fd);
    free(buf);
    free(expanded_destination_filepath);

    return 0;
}