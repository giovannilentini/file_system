#pragma once

#define FILE_SIS_SIZE 1024 * 1024
#define BLOCK_SIZE 512
#define MAX_BLOCKS FILE_SIS_SIZE/BLOCK_SIZE
#define FAT_FREE 0
#define MY_EOF -1

typedef struct {
    char name[48];
    int first_block;
    int size;
    int is_directory;
    int current_position;
} FileEntry;

/*
    Initializes the file system, setting up the FAT, 
    file entries, and data blocks in an mmapped file.

    name: name of the file image to open or create.
*/
void init(const char *name);

/*
    Erases the entire disk, effectively formatting the file system.
*/
void erase_disk();

/*
    Gets the name of the current working directory.
    Returns the name of the current working directory.
*/
const char* get_current_dir_name();

/*
    Gets the File Control Block (FCB) for the specified file index.
    Returns the pointer to the FileEntry structure for the file.

    name: The name of the file or directory.
*/
FileEntry* open_file_entry(const char* name);

/*
    Creates a new file with the specified name. Returns the 
    index of the new file in the file entries, or -1 if no 
    free file entry is found.
    Returns The index of the new file, or -1 on failure.

    filename: The name of the file to create.
*/
int create_file(const char *filename);

/*
    Writes data to a file, potentially extending its size. Returns
    the number of bytes written.
    Returns the number of bytes written, or -1 on failure.

    filename: The name of the file to write to;
    buffer: The buffer containing the data to write;
    size: The size of the data to write.
*/
int write_file(const char *filename, const char *buffer, int size);

/*
    Reads data from a file. Returns the number of bytes read.
    Returns the number of bytes read, or -1 on failure.

    filename: The name of the file to read from;
    buffer:  The buffer to store the read data;
    size: The size of the data to read.
*/
int read_file(const char *filename, char *buffer, int size);

/*
    Sets the position in a file handle for subsequent read/write 
    operations.
    Returns 0 on success, -1 on failure.

    filename: The name of the file to set the position in.
    position: The position to seek to.
*/
int seek(const char *filename, int offset);

/*
    Erases a file or an empty dir by freeing its blocks in the FAT and clearing is entry.
    Returns 0 on success, -1 on failure.

    filename: name of the file or directory to delete.
*/
int erase_file(const char *filename);

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
int change_dir(const char *dirname);

/*
    Lists the file and directories in the current directory.
*/
void ls_dir();

/*
    Deletes a directory and all its contents recursively. If the 
    directory contains other directories, they are deleted along 
    with their contents.
    Returns 0 on success, -1 on failure.

    dirname: name of the direcorty to delete.
*/
int erase_dir_recursive(const char *dirname);

/*
    Copies a file from the main filesystem to the custom filesystem.
    Returns 0 on success, or -1 on failure.

    source_filepath: The path of the source file to copy;
    destination_filename: The name of the file to create in the custom filesystem.
*/
int copy_to_my_fs(const char *source_filepath, const char *destination_filename);

/*
    Copies a file from the custom filesystem to the main filesystem.
    Returns 0 on success, or -1 on failure.

    source_filename: The name of the file to copy from the custom filesystem;
    destination_filepath: The path where the file should be saved in the main filesystem.
*/
int copy_from_my_fs(const char *source_filename, const char *destination_filepath);
