#include <stdio.h>

#include "../include/file_system.h"
#include "../include/shell.h"

int main() {
    init("shell/shell_disk.img");

    do_command();
}