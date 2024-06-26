# File System Similar to FAT

This project implements a simple file system inspired by the structure of a File Allocation Table (FAT). It is written in C and uses an image file (`test_disk.img`) as persistent storage support. The project includes various functionalities for file and directory management, based on data blocks and a file allocation table (FAT).

## Implemented Features

The file system includes the following key features:

### File and Directory Creation and Management:

- Creation of files and directories.
- Deletion of files and directories (including recursively).
- Changing and navigating current directories.
- Viewing directory contents.

### File Operations:

- Writing and reading data from files.
- Moving the current position within a file (seek).

### Additional Utilities:

- Initialization and formatting of the disk (`init`, `erase_disk`).
- Execution of commands via an interactive shell (`main_shell`).

## How to Use

To view and test the functionality of the file system, you can run the `fs.sh` script. If you prefer to compile and execute manually:

1. Run `make` from the project's main directory.
2. The resulting executables will be located in the `test` folder:

   - `./dir_test`: Tests directory operations.
   - `./write_read_test`: Tests write and read operations.
   - `./seek_test`: Tests seek operation.
   - `./erase_test`: Tests file and directory deletion.

Additionally, an interactive command-line interface (`main_shell`) is available in the `shell` folder, allowing interaction with the file system by executing commands directly from the console.

## Clean Up

To remove the generated executables during compilation, execute:

```bash
make clean
```

## Contributions

This project was developed as part of an educational exercise to understand the fundamentals of file systems and the low-level operations involved in their implementation.

For more details and to explore the source code, visit the GitHub repository.