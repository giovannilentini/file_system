#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "../include/file_system.h"

void *fs;
int *FAT;
FileEntry *file_entries;
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

int find_file_index(const char *name) {
    for (int i = 0; i < MAX_FILE; i++) {
        if (file_entries[i].parent_index == current_dir_index && strcmp(file_entries[i].name, name) == 0) {
            return i;
        }
    }
    printf("File or Directory '%s' not found in the current directory.\n", name);
    return -1;
}

/* ===== DISK FUNCTION ===== */

void init(const char *name) {
    int fd = open(name, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    ftruncate(fd, FILE_SIS_SIZE);

    fs = mmap(NULL, FILE_SIS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fs == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(1);
    }
    close(fd);

    FAT = (int*) fs;
    file_entries = (FileEntry*) (FAT + (FILE_SIS_SIZE / BLOCK_SIZE));
    data_blocks = (char*) (file_entries + MAX_FILE);

    if (file_entries[0].first_block == FAT_FREE) {
        strcpy(file_entries[0].name, "/");
        file_entries[0].first_block = MY_EOF;
        file_entries[0].size = 0;
        file_entries[0].is_directory = 1;
        file_entries[0].parent_index = -1;
        file_entries[0].first_child = -1;
        file_entries[0].next_sibling = -1;

        for (int i = 1; i < MAX_FILE; i++) {
            file_entries[i].first_block = FAT_FREE;
        }
        for (int i = 0; i < MAX_BLOCKS; i++) {
            FAT[i] = FAT_FREE;
        }

        current_dir_index = 0;
        sync_fs();
    } else {
        current_dir_index = 0;
    }

}

void erase_disk() {
    memset(FAT, FAT_FREE, FILE_SIS_SIZE / BLOCK_SIZE * sizeof(int));
    memset(file_entries, 0, MAX_FILE * sizeof(FileEntry));

    strcpy(file_entries[0].name, "/");
    file_entries[0].is_directory = 1;
    file_entries[0].first_block = MY_EOF;
    file_entries[0].parent_index = -1;
    file_entries[0].first_child = -1;
    file_entries[0].next_sibling = -1;

    sync_fs();
}

/* ===== FILE FUNCTION ===== */

int create_file(const char *filename) {
    for (int i = 0; i < MAX_FILE; i++) {
        if (file_entries[i].parent_index == current_dir_index && strcmp(file_entries[i].name, filename) == 0) {
            printf("File or directory '%s' already exists in the current directory.\n", filename);
            return -1;
        }
    }

    for (int i = 0; i < MAX_FILE; i++) {
        if (file_entries[i].first_block == FAT_FREE) {
            strcpy(file_entries[i].name, filename);
            file_entries[i].first_block = allocate_block();
            if (file_entries[i].first_block == FAT_FREE) {
                printf("Failed to allocate block for file '%s'\n", filename);
                return -1;
            }
            file_entries[i].size = 0;
            file_entries[i].is_directory = 0;
            file_entries[i].parent_index = current_dir_index;
            file_entries[i].first_child = -1;
            file_entries[current_dir_index].first_child = i;
            file_entries[i].next_sibling = file_entries[current_dir_index].first_child;
            
            sync_fs();
            return i;
        }
    }
    printf("Maximum number of files reached.\n");
    return -1;
}

int write_file(const char *filename, const char *buffer, int size) {
    int file_index = find_file_index(filename);
    if (file_index == -1) {
        return -1;
    }

    int current_block = file_entries[file_index].first_block;
    int remaining_size = size;
    int offset = 0;

    while (remaining_size > 0) {
        int to_write = (remaining_size < BLOCK_SIZE) ? remaining_size : BLOCK_SIZE;
        memcpy(data_blocks + current_block * BLOCK_SIZE, buffer + offset, to_write);
        remaining_size -= to_write;
        offset += to_write;

        if (remaining_size > 0) {
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
        }
    }
    file_entries[file_index].size += size;

    sync_fs();
    return size;
}

int read_file(const char *filename, char *buffer, int size) {
    int file_index = find_file_index(filename);
    if (file_index == -1) {
        return -1;
    }

    int current_block = file_entries[file_index].first_block;
    int remaining_size = size;
    int offset = 0;

    while (remaining_size > 0 && current_block != MY_EOF) {
        int to_read = (remaining_size < BLOCK_SIZE) ? remaining_size : BLOCK_SIZE;
        memcpy(buffer + offset, data_blocks + current_block * BLOCK_SIZE, to_read);
        remaining_size -= to_read;
        offset += to_read;
        current_block = FAT[current_block];
    }

    sync_fs();
    return offset;
}

void erase_file(const char *filename) {
    int file_index = find_file_index(filename);
    if (file_index == -1) {
        return;
    }

    if (file_entries[file_index].is_directory) {
        if (file_entries[file_index].first_child != -1) {
            printf("Directory '%s' is not empty.\n", file_entries[file_index].name);
            return;
        }
    } else {

        int current_block = file_entries[file_index].first_block;
        while (current_block != MY_EOF) {
            int next_block = FAT[current_block];
            FAT[current_block] = FAT_FREE;
            current_block = next_block;
        }
    }

    file_entries[file_index].first_block = FAT_FREE;
    file_entries[file_index].name[0] = '\0';
    file_entries[file_index].first_child = -1;
    file_entries[file_index].next_sibling = -1;

    sync_fs();
}

/* ===== DIRECTORY FUNCTION ===== */

int create_dir(const char *dirname) {
    for (int i = 0; i < MAX_FILE; i++) {
        if (file_entries[i].parent_index == current_dir_index && strcmp(file_entries[i].name, dirname) == 0) {
            printf("File or directory '%s' already exists in the current directory.\n", dirname);
            return -1;
        }
    }

    for (int i = 0; i < MAX_FILE; i++) {
        if (file_entries[i].first_block == FAT_FREE) {
            strcpy(file_entries[i].name, dirname);
            file_entries[i].first_block = MY_EOF;
            file_entries[i].size = 0;
            file_entries[i].is_directory = 1;
            file_entries[i].parent_index = current_dir_index;
            file_entries[i].first_child = -1;
            file_entries[i].next_sibling = file_entries[current_dir_index].first_child;
            file_entries[current_dir_index].first_child = i;
            
            sync_fs();
            return i;
        }
    }

    printf("Maximum number of directories reached.\n");
    return -1;
}

int change_dir(const char *dirname) {
    if (strcmp(dirname, "..") == 0) {
        if (current_dir_index == 0) {
            printf("Already at root directory.\n");
            return -1;
        } else if (file_entries[current_dir_index].parent_index != -1) {
            current_dir_index = file_entries[current_dir_index].parent_index;
            return 0;
        } else {
            printf("Cannot move up from current directory.\n");
            return -1;
        }
        return -1;
    } else {
        int found = 0;
        for (int i = 0; i < MAX_FILE; i++) {
            if (file_entries[i].parent_index == current_dir_index && file_entries[i].is_directory && strcmp(file_entries[i].name, dirname) == 0) {
                current_dir_index = i;
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("Directory '%s' not found in current directory.\n", dirname);
            return -1;
        }
        
        return 0;
    }
}

void ls_dir() {
    printf("Listing directory contents for directory index %d:\n", current_dir_index);
    for (int i = 0; i < MAX_FILE; i++) {
        if (file_entries[i].parent_index == current_dir_index && file_entries[i].name[0] != '\0') {
            if (file_entries[i].is_directory) {
                printf("Dir: %s\n", file_entries[i].name);
            } else {
                printf("File: %s\n", file_entries[i].name);
            }
        }
    }
}
