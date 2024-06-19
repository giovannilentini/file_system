#include "stdio.h"

#include "../include/file_system.h"

int main() {
    init("bin/test_disk.img");

    printf("\n********************\ncurrent dir: root\n");

    create_file("file_1_root.txt");
    create_file("file_2_root.txt");
    create_file("file_3_root.txt");

    create_dir("dir 1");
    create_dir("dir 2");

    ls_dir();

    change_dir("dir 1");
    printf("\n********************\ncurrent dir: dir 1\n");

    create_file("file_1_dir1.txt");
    create_file("file_2_dir1.txt");
    create_dir("dir 1_1");
    ls_dir();

    change_dir("dir 1_1");
    printf("\n********************\ncurrent dir: dir 1_1\n");

    create_file("file_1_dir1_1.txt");
    create_file("file_2_dir1_1.txt");

    ls_dir();
    
    change_dir("..");
    printf("\n********************\ncurrent dir: dir 1\n");
    ls_dir();

    change_dir("..");
    printf("\n********************\ncurrent dir: root\n");
    ls_dir();

    return 0;
}