#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "../include/file_system.h"

void *fs;
int *FAT;
char *data_blocks;
int current_dir_index = 0;

/* ===== HANDLE FUNCTION ===== */

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

/* ===== DIRECTORY FUNCTION ===== */

int create_dir(const char *dirname) {
    int current_block = current_dir_index;
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
    int current_block = current_dir_index;

    while (current_block != MY_EOF) {
        FileEntry *current_dir = (FileEntry *)(data_blocks + current_block * BLOCK_SIZE);

        for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
            FileEntry *entry = current_dir + i;
            if (entry->name[0] != '\0') {
                printf("%s: %s\n", entry->is_directory ? "Dir" : "File", entry->name);
            }
        }

        current_block = FAT[current_block];
    }
}
