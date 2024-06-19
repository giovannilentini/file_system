#include <stdio.h>

#include "../include/file_system.h" 

int main() {
    init("bin/test_disk.img");
    printf("\n********************\ncurrent dir: root\n");

    //create_file("file_1_root.txt");

    write_file("file_2_root.txt", "ciao", 4);
    char buf[5];
    read_file("file_2_root.txt", buf, 4);
    printf("read = %s\n\n", buf); 

    change_dir("dir 1");
    ls_dir();
}