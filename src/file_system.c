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
            FileEntry *file_entry = current_dir + i;

            if (file_entry->name[0] == '\0') {
                strcpy(file_entry->name, filename);
                file_entry->first_block = allocate_block();
                if (file_entry->first_block == -1) {
                    printf("Error: Failed to allocate block for file '%s'\n", filename);
                    return -1;
                }
                file_entry->size = 0;
                file_entry->is_directory = 0;
                file_entry->current_position = 0;

                sync_fs();
                return i;
            }
        }

        int next_block = FAT[current_block];
        if (next_block == MY_EOF) {
            int new_block = allocate_block();
            if (new_block == -1) {
                printf("Error: Failed to allocate block for new directory block\n");
                return -1;
            }
            FAT[current_block] = new_block;
            FAT[new_block] = MY_EOF;
            current_block = new_block;

            FileEntry *new_dir_block = (FileEntry *)(data_blocks + new_block * BLOCK_SIZE);
            memset(new_dir_block, 0, BLOCK_SIZE);
            sync_fs();
        } else {
            current_block = next_block;
        }
    }

    printf("Error: Failed to create file '%s'\n", filename);
    return -1;
}

/* ===== DIRECTORY FUNCTION ===== */

int create_dir(const char *dirname) {
    int current_block = current_dir_index;

    while (current_block != MY_EOF) {
        FileEntry *current_dir = (FileEntry *)(data_blocks + current_block * BLOCK_SIZE);

        for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
            FileEntry *dir_entry = current_dir + i;
            
            if (dir_entry->name[0] == '\0') {
                strcpy(dir_entry->name, dirname);
                dir_entry->first_block = allocate_block();
                if (dir_entry->first_block == -1) {
                    printf("Error: Failed to allocate block for directory '%s'\n", dirname);
                    return -1;
                }
                dir_entry->size = 0;
                dir_entry->is_directory = 1;
                dir_entry->current_position = 0;

                FileEntry *new_dir = (FileEntry *)(data_blocks + dir_entry->first_block * BLOCK_SIZE);
                memset(new_dir, 0, BLOCK_SIZE);

                sync_fs();
                return i;
            }
        }

        int next_block = FAT[current_block];
        if (next_block == MY_EOF) {
            int new_block = allocate_block();
            if (new_block == -1) {
                printf("Error: Failed to allocate block for new directory block\n");
                return -1;
            }
            FAT[current_block] = new_block;
            FAT[new_block] = MY_EOF;
            current_block = new_block;

            FileEntry *new_dir_block = (FileEntry *)(data_blocks + new_block * BLOCK_SIZE);
            memset(new_dir_block, 0, BLOCK_SIZE);
            sync_fs();
        } else {
            current_block = next_block;
        }
    }

    printf("Error: Failed to create directory '%s'\n", dirname);
    return -1;
}

int change_dir(const char *dirname) {
    if (strcmp(dirname, "..") == 0) {
        if (current_dir_index == 0) {
            printf("Already at root directory.\n");
            return -1;
        } else {
            for (int i = 0; i < MAX_BLOCKS; i++) {
                FileEntry *entry = (FileEntry *)(data_blocks + i * BLOCK_SIZE);
                for (int j = 0; j < BLOCK_SIZE / sizeof(FileEntry); j++) {
                    FileEntry *sub_entry = entry + j;
                    if (sub_entry->is_directory && sub_entry->first_block == current_dir_index) {
                        current_dir_index = i;
                        return 0;
                    }
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
    printf("Listing directory contents for directory '%s':\n", ((FileEntry *)(data_blocks + current_block * BLOCK_SIZE))->name);

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

