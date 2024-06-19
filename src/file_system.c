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

void init(const char *name) {
    int fd = open(name, O_RDWR | O_CREAT, 0600);
    ftruncate(fd, FILE_SIS_SIZE);

    fs = mmap(NULL, FILE_SIS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    FAT = (int*) fs;
    file_entries = (FileEntry*) (FAT + (FILE_SIS_SIZE / BLOCK_SIZE));
    data_blocks = (char*) (file_entries + MAX_FILE);

    memset(FAT, FAT_FREE, FILE_SIS_SIZE / BLOCK_SIZE * sizeof(int));
    memset(file_entries, 0, MAX_FILE * sizeof(FileEntry));

    strcpy(file_entries[0].name, "/");
    file_entries[0].is_directory = 1;
    file_entries[0].first_block = MY_EOF;
    file_entries[0].parent_index = -1;
    file_entries[0].first_child = -1;
    file_entries[0].next_sibling = -1;

    msync(fs, FILE_SIS_SIZE, MS_SYNC);

}

/*
    Syncronize the changes in the disk image.
*/
void sync_fs() {
    msync(fs, FILE_SIS_SIZE, MS_SYNC);
}

int allocate_block() {
    for (int i = 0; i < FILE_SIS_SIZE / BLOCK_SIZE; i++) {
        if (FAT[i] == FAT_FREE) {
            FAT[i] = MY_EOF;
            return i;
        }
    }
    return -1;
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
        if (file_entries[current_dir_index].parent_index != -1) {
            current_dir_index = file_entries[current_dir_index].parent_index;
            return 0;
        }
        return -1;
    } else {
        for (int i = file_entries[current_dir_index].first_child; i != -1; i = file_entries[i].next_sibling) {
            if (file_entries[i].is_directory && strcmp(file_entries[i].name, dirname) == 0) {
                current_dir_index = i;
                return 0;
            }
        }
        printf("Directory '%s' not found in current directory.\n", dirname);
        return -1;
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