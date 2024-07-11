#include "stdio.h"

#include "../include/file_system.h"

int main() {
    init("bin/test_disk.img");

    printf("\n********************\ncurrent dir: root\n\n");
    create_file("file_1_root.txt");
    create_file("file_2_root.txt");
    create_file("file_3_root.txt");
    create_dir("dir 1");
    create_dir("dir 2");
    ls_dir();
    printf("********************\n");

    /*
    init("bin/test_disk.img");
    erase_disk();

    printf("\n********************\ncurrent dir: root\n\n");
    create_file("file_1_root.txt");
    create_file("file_2_root.txt");
    create_file("file_3_root.txt");
    create_dir("dir 1");
    create_dir("dir 2");
    ls_dir();
    printf("********************\n");

    change_dir("dir 1");
    printf("\n********************\ncurrent dir: dir 1\n\n");
    create_file("file_1_dir1.txt");
    create_file("file_2_dir1.txt");
    create_dir("dir 1_1");
    ls_dir();
    printf("********************\n");

    change_dir("dir 1_1");
    printf("\n********************\ncurrent dir: dir 1_1\n\n");
    create_file("file_1_dir1_1.txt");
    create_file("file_2_dir1_1.txt");
    ls_dir();
    printf("********************\n");
    
    change_dir("..");
    printf("\n********************\ncurrent dir: dir 1\n\n");
    ls_dir();
    printf("********************\n");

    change_dir("..");
    printf("\n********************\ncurrent dir: root\n\n");
    ls_dir();
    printf("********************\n");

    printf("\n********************\nother test on change_dir\n\n");
    change_dir("..");
    change_dir("nonexistentdir");
    change_dir("dir 1_1");
    printf("********************\n\n");
    */
    return 0;
}