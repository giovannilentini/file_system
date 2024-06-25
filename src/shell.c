#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/shell.h"
#include "../include/file_system.h"

char command[MAX_COMMAND_LEN];

void format(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 1) {
        printf("format error: wrong number of parameters.\n");
        return;
    }

    change_dir_root();
    erase_disk();
}

void touch(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 2) {
        printf("touch error: wrong number of parameters.\n");
        return;
    }

    create_file(argv[1]);
}

void mkdir(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 2) {
        printf("mkdir error: wrong number of parameters.\n");
        return;
    }

    create_dir(argv[1]);
}

void rm(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 2) {
        printf("rm error: wrong number of parameters.\n");
        return;
    }

    erase_file(argv[1]);
}

void rmrf(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 2) {
        printf("rmrf error: wrong number of parameters.\n");
        return;
    }

    erase_dir_recursive(argv[1]);
}

void ls(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 1) {
        printf("ls error: wrong number of parameters.\n");
        return;
    }

    ls_dir();
}

void cd(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 2) {
        printf("cd error: wrong number of parameters.\n");
        return;
    }

    change_dir(argv[1]);
}

void write(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 2) {
        printf("write error: wrong number of parameters.\n");
        return;
    }

    if (seek(argv[1], 0) < 0) return;

    int position;
    
    printf("Enter starting point: ");
    scanf("%d", &position);
    getchar();

    if (seek(argv[1], position) < 0) return;
    
    printf("Enter text: ");
    char* str = calloc(2048, sizeof(char));
    fgets(str, 2048, stdin);
    str[strlen(str) - 1] = 0;
    write_file(argv[1], str, strlen(str));
}

void read(int argc, char* argv[MAX_ARGC + 1]) {

    if (argc != 2) {
        printf("read error: wrong number of parameters \n");
        return;
    }

    if (seek(argv[1], 0) < 0) return;

    int position;
    
    printf("Enter starting point: ");
    scanf("%d", &position);
    getchar();

    if (seek(argv[1], position) < 0) return;
 
    char* str = calloc(2048, sizeof(char));
    read_file(argv[1], str, 2048);
    printf("%s\n", str);
    
}

void do_command() {

    while (1) {
        
        char* argv[MAX_ARGC+1];
        FileEntry current_dir_fcb = get_fcb();
        printf("%s> ", current_dir_fcb.name);
        fgets(command, 128, stdin);
        
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
        } else if (strcmp(argv[0], "exit") == 0 || strcmp(argv[0], "quit") == 0) {
            exit(EXIT_SUCCESS);
        } else {
            printf("erroreeee\n");
        }
    }
    
} 
