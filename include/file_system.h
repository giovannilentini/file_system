#pragma once

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define FILE_SIS_SIZE 1024 * 1024
#define BLOCK_SIZE 512
#define MAX_BLOCKS FILE_SIS_SIZE/BLOCK_SIZE
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

/*
    Initializes the file system, setting up the FAT, 
    file entries, and data blocks in an mmapped file.

    name: name of the file image to open or create.
*/
void init(const char *name);

void erase_disk();

/*
    Creates a new file with the specified name. Returns the 
    index of the new file in the file entries, or -1 if no 
    free file entry is found.

    filename: The name of the file to create.
*/
int create_file(const char *filename);

/*
    Writes data to a file, potentially extending its size. Returns
    the number of bytes written.

    filename: The name of the file to write to;
    buffer: The buffer containing the data to write;
    size: The size of the data to write.
*/
int write_file(const char *filename, const char *buffer, int size);

/*
    Reads data from a file. Returns the number of bytes read.

    filename: The name of the file to read from;
    buffer:  The buffer to store the read data;
    size: The size of the data to read.
*/
int read_file(const char *filename, char *buffer, int size);

/*
    Erases a file or an empty dir by freeing its blocks in the FAT and clearing is entry.

    filename: name of the file or directory to delete.
*/
void erase_file(const char *filename);

/*
    Creates a new directory with the specified name. Returns the
    index of the new directory in the file entries, or -1 if no free 
    directory entry is found.

    dirname: The name of the directory to create.
*/
int create_dir(const char *dirname);

/*
    Changes the current working directory to the specified directory. 
    Returns 0 if successful, or -1 if the directory is not
    found or is not a directory.

    dirname: The name of the directory to change to.
*/
int change_dir(const char *path);

/*
    Lists the file and directories in the current directory.
*/
void ls_dir();
