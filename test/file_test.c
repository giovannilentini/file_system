#include "stdio.h"
#include <stdlib.h>

#include "../include/file_system.h"

void int_to_string(int num, char *str) {
    sprintf(str, "%d", num);
}

int main() {

    init("bin/file_test_disk.img");

    char name[4] = {0}, buf[256] = {0};
    
    for (int i = 0; i < 255; i++) {
        int_to_string(i, name);
        create_file(name);

        sprintf(buf, "scrivo nel file %d", i);
        write_file(name, buf, 8);

        read_file(name, buf, 256);
        printf("%s\n", buf);
    }

    ls_dir();
    erase_disk();

    return 0;
}