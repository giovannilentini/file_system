#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/shell.h"
#include "../include/file_system.h"

char command[MAX_COMMAND_LEN];

void format(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 1) {
        printf("Error: wrong number of parameters.\n");
        printf("Usage: format\n");
        return;
    }

    erase_disk();
}

void touch(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 2) {
        printf("Error: wrong number of parameters.\n");
        printf("Usage: touch <filename>\n");
        return;
    }

    create_file(argv[1]);
}

void mkdir(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 2) {
        printf("Error: wrong number of parameters.\n");
        printf("Usage: mkdir <dirname>\n");
        return;
    }

    create_dir(argv[1]);
}

void rm(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 2) {
        printf("Error: wrong number of parameters.\n");
        printf("Usage: rm <filename>\n");
        return;
    }

    erase_file(argv[1]);
}

void rmrf(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 2) {
        printf("Error: wrong number of parameters.\n");
        printf("Usage: rmrf <dirname>\n");
        return;
    }

    erase_dir_recursive(argv[1]);
}

void ls(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 1) {
        printf("Error: wrong number of parameters.\n");
        printf("Usage: ls\n");
        return;
    }

    ls_dir();
}

void cd(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 2) {
        printf("Error: wrong number of parameters.\n");
        printf("Usage: cd <dirname>\n");
        return;
    }

    change_dir(argv[1]);
}

void write(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 2) {
        printf("Error: wrong number of parameters.\n");
        printf("Usage: write <filename>\n");
        return;
    }

    char *filename = argv[1];

    if (open_file_entry(filename)->is_directory) {
        printf("Error: cannot write a directory.\n");
        return;
    }

    int position;
    
    printf("Enter starting point: ");
    scanf("%d", &position);
    getchar();

    if (seek(filename, position) < 0) return;
    
    printf("Enter text: ");
    char* str = calloc(2048, sizeof(char));
    fgets(str, 2048, stdin);
    //str[strlen(str) - 1] = 0;
    write_file(filename, str, strlen(str)-1);
}

void read(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 2) {
        printf("Error: wrong number of parameters \n");
        printf("Usage: read <filename>\n");
        return;
    }

    char *filename = argv[1];

    if (open_file_entry(filename)->is_directory) {
        printf("Error: cannot read a directory.\n");
        return;
    }

    int position;
    
    printf("Enter starting point: ");
    scanf("%d", &position);
    getchar();

    if (seek(filename, position) < 0) return;
 
    char* str = calloc(2048, sizeof(char));
    read_file(filename, str, 2048);
    printf("%s\n", str);
    
}

void clear(int argc, char* argv[MAX_ARGC + 1]) {
    if (argc != 1) {
        printf("Error: wrong number of parameters.\n");
        printf("Usage: clear\n");
        return;
    }

    system("clear");
    
}

void help(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 1) {
        printf("Error: wrong number of parameters \n");
        printf("Usage: help\n");
        return;
    }

    printf("Available commands:\n\n");

    printf(" 1. format\n");
    printf("    Usage: format\n");
    printf("    Description: Formats the disk.\n\n");

    printf(" 2. touch\n");
    printf("    Usage: touch <filename>\n");
    printf("    Description: Creates a new file with the specified name.\n\n");

    printf(" 3. mkdir\n");
    printf("    Usage: mkdir <dirname>\n");
    printf("    Description: Creates a new directory with the specified name.\n\n");

    printf(" 4. rm\n");
    printf("    Usage: rm <filename>\n");
    printf("    Description: Removes the specified file.\n\n");

    printf(" 5. rmrf\n");
    printf("    Usage: rmrf <dirname>\n");
    printf("    Description: Recursively removes the specified directory and its contents.\n\n");

    printf(" 6. ls\n");
    printf("    Usage: ls\n");
    printf("    Description: Lists the contents of the current directory.\n\n");

    printf(" 7. cd\n");
    printf("    Usage: cd <dirname>\n");
    printf("    Description: Changes the current directory to the specified directory.\n\n");

    printf(" 8. write\n");
    printf("    Usage: write <filename>\n");
    printf("    Description: Writes to the specified file starting at a specified position.\n\n");

    printf(" 9. read\n");
    printf("    Usage: read <filename>\n");
    printf("    Description: Reads from the specified file starting at a specified position.\n\n");

    printf("10. clear\n");
    printf("    Usage: clear\n");
    printf("    Description: Clears the terminal screen.\n\n");

    printf("11. help\n");
    printf("    Usage: help\n");
    printf("    Description: Displays this help message.\n\n");
}

void do_command() {
    while (1) {
        
        char* argv[MAX_ARGC+1];

        printf("%s> ", get_current_dir_name());
        fgets(command, MAX_COMMAND_LEN, stdin);
        
    
        argv[0] = strtok(command, " ");
        argv[1] = strtok(NULL, "\n");

        int argc = (argv[1] == NULL) ? 1 : 2;

        if (argv[1] == NULL) {
            argv[0][strlen(argv[0]) - 1] = 0;
        }

        if (strcmp(argv[0], "format") == 0) {
            format(argc, argv);
        } else if (strcmp(argv[0], "touch") == 0) {
            touch(argc, argv);
        } else if (strcmp(argv[0], "mkdir") == 0) {
            mkdir(argc, argv);
        } else if (strcmp(argv[0], "rm") == 0) {
            rm(argc, argv);
        } else if (strcmp(argv[0], "rmrf") == 0) {
            rmrf(argc, argv);
        } else if (strcmp(argv[0], "ls") == 0) {
            ls(argc, argv);
        } else if (strcmp(argv[0], "cd") == 0) {
            cd(argc, argv);
        } else if (strcmp(argv[0], "write") == 0) {
            write(argc, argv);
        } else if (strcmp(argv[0], "read") == 0) {
            read(argc, argv);
        } else if (strcmp(argv[0], "clear") == 0) {
            clear(argc, argv);
        } else if (strcmp(argv[0], "help") == 0) {
            help(argc, argv);
        }  else if (strcmp(argv[0], "exit") == 0 || strcmp(argv[0], "quit") == 0) {
            exit(EXIT_SUCCESS);
        } else {
            printf("Error: %s command not found\n", command);
        }
    }
}
