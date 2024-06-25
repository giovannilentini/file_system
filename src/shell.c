#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/shell.h"
#include "../include/file_system.h"

char command[128];


void do_command() {

    while (1) {
        
        char* argv[3];
        FileEntry current_dir_fcb = get_fcb();
        printf("%s> ", current_dir_fcb.name);
        fgets(command, 128, stdin);
        
        argv[0] = strtok(command, " ");
        argv[1] = strtok(NULL, "\n");

        if (argv[1] == NULL) {
            argv[0][strlen(argv[0]) - 1] = 0;
        }

        if (strcmp(argv[0], "format") == 0) {
            erase_disk();
        } else if (strcmp(argv[0], "touch") == 0) {
            create_file(argv[1]);
        } else if (strcmp(argv[0], "mkdir") == 0) {
            create_dir(argv[1]);
        } else if (strcmp(argv[0], "rm") == 0) {
            erase_file(argv[1]);
        } else if (strcmp(argv[0], "rmrf") == 0) {
            erase_dir_recursive(argv[1]);
        } else if (strcmp(argv[0], "ls") == 0) {
            ls_dir();
        } else if (strcmp(argv[0], "cd") == 0) {
            change_dir(argv[1]);
        } else if (strcmp(argv[0], "exit") == 0 || strcmp(argv[0], "quit") == 0) {
            exit(EXIT_SUCCESS);
        } else {
            printf("erroreeee\n");
        }
    }
    
}