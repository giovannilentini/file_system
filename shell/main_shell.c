#include <stdio.h>

#include "../include/file_system.h"
#include "../include/shell.h"

int main() {
    init("shell/shell_disk.img");

    printf("Welcome, run 'help' to see available commands!\n");
    do_command();
}