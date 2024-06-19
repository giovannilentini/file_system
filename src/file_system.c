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

void init(const char *name) {
    int fd = open("fs.img", O_RDWR | O_CREAT, 0600);
    ftruncate(fd, FILE_SIS_SIZE);

    fs = mmap(NULL, FILE_SIS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    FAT = (int*) fs;
    file_entries = (FileEntry*) (FAT + (FILE_SIS_SIZE / BLOCK_SIZE));
    data_blocks = (char*) (file_entries + MAX_FILE);

    memset(FAT, FAT_FREE, FILE_SIS_SIZE / BLOCK_SIZE * sizeof(int));
    memset(file_entries, 0, MAX_FILE * sizeof(FileEntry));

    // here i'm creating the root directory
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