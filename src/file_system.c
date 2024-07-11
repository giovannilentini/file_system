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

/* ===== FILE FUNCTION ===== */

int create_file(const char *filename) {
    int dir_block = current_dir_index;
    while (dir_block != MY_EOF) {
        for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
            FileEntry *entry = (FileEntry *)(data_blocks + dir_block * BLOCK_SIZE + i * sizeof(FileEntry));

            if (entry->first_block != FAT_FREE && strcmp(entry->name, filename) == 0) {
                printf("Error: File or Directory '%s' already exists in the current directory.\n", filename);
                return -1;
            }

            if (entry->first_block == FAT_FREE) {
                int block = allocate_block();
                if (block == -1) {
                    printf("Error: No more blocks available.\n");
                    return -1;
                }

                strcpy(entry->name, filename);
                entry->first_block = block;
                entry->size = 0;
                entry->is_directory = 0;
                entry->current_position = 0;

                sync_fs();
                return dir_block * (BLOCK_SIZE / sizeof(FileEntry)) + i;
            }
        }

        if (FAT[dir_block] == MY_EOF) {
            int new_block = allocate_block();
            if (new_block == -1) {
                printf("Error: No more blocks available.\n");
                return -1;
            }
            FAT[dir_block] = new_block;
            FAT[new_block] = MY_EOF;
        }
        dir_block = FAT[dir_block];
    }

    return -1;
}

/* ===== DIRECTORY FUNCTION ===== */

int create_dir(const char *dirname) {
    int dir_block = current_dir_index;
    while (dir_block != MY_EOF) {
        for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
            FileEntry *entry = (FileEntry *)(data_blocks + dir_block * BLOCK_SIZE + i * sizeof(FileEntry));

            if (entry->first_block != FAT_FREE && strcmp(entry->name, dirname) == 0) {
                printf("Error: File or Directory '%s' already exists in the current directory.\n", dirname);
                return -1;
            }

            if (entry->first_block == FAT_FREE) {
                int block = allocate_block();
                if (block == -1) {
                    printf("Error: No more blocks available.\n");
                    return -1;
                }

                strcpy(entry->name, dirname);
                entry->first_block = block;
                entry->size = 0;
                entry->is_directory = 1;
                entry->current_position = 0;

                sync_fs();
                return dir_block * (BLOCK_SIZE / sizeof(FileEntry)) + i;
            }
        }

        if (FAT[dir_block] == MY_EOF) {
            int new_block = allocate_block();
            if (new_block == -1) {
                printf("Error: No more blocks available.\n");
                return -1;
            }
            FAT[dir_block] = new_block;
            FAT[new_block] = MY_EOF;
        }
        dir_block = FAT[dir_block];
    }

    return -1;
}

void ls_dir() {
    int dir_block = current_dir_index;
    while (dir_block != MY_EOF) {
        for (int i = 0; i < BLOCK_SIZE / sizeof(FileEntry); i++) {
            FileEntry *entry = (FileEntry *)(data_blocks + dir_block * BLOCK_SIZE + i * sizeof(FileEntry));

            if (entry->first_block != FAT_FREE) {
                printf("%s: '%s'\n", entry->is_directory ? "Dir" : "File", entry->name);
            }
        }

        dir_block = FAT[dir_block];
    }
}
