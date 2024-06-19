#pragma once

#define FILE_SIS_SIZE 1024 * 1024
#define BLOCK_SIZE 512
#define MAX_FILE 128 
#define FAT_FREE 0
#define MY_EOF -1

typedef struct {
    char name[64];
    int first_block;
    int size;
    int is_directory;
    int parent_index;
    int first_child;
    int next_sibling;
} FileEntry;

void init(const char *name);