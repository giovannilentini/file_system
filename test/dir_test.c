#include <stdio.h>

#include "../include/file_system.h"

int main() {

    init("bin/test_disk.img");
    erase_disk();

    printf("\n********************\ncurrent dir: root\n\n");
    create_file("file_1_root.txt");
    create_file("file_2_root.txt");
    create_file("file_3_root.txt");
    create_file("file_4_root.txt");
    create_file("file_5_root.txt");
    create_file("file_6_root.txt");
    create_file("file_7_root.txt");
    create_file("file_8_root.txt");
    create_file("file_9_root.txt");
    create_file("file_10_root.txt");
    create_file("file_11_root.txt");
    create_file("file_12_root.txt");
    create_dir("dir 1");
    create_dir("dir 2");
    create_dir("dir 3");
    create_dir("dir 4");
    create_dir("dir 5");
    create_dir("dir 6");
    create_dir("dir 7");
    create_dir("dir 8");
    create_dir("dir 9");
    create_dir("dir 10");
    create_dir("dir 11");
    create_dir("dir 12");
    ls_dir();
    printf("********************\n");

    change_dir("dir 1");
    printf("\n********************\ncurrent dir: dir 1\n\n");
    create_file("file_1_dir1.txt");
    create_file("file_2_dir1.txt");
    create_dir("dir 1_1");
    create_file("file_3_dir1.txt");
    create_file("file_4_dir1.txt");
    create_file("file_5_dir1.txt");
    create_file("file_6_dir1.txt");
    create_file("file_7_dir1.txt");
    create_file("file_8_dir1.txt");
    create_file("file_9_dir1.txt");
    create_file("file_10_dir1.txt");
    create_file("file_11_dir1.txt");
    create_file("file_12_dir1.txt");
    create_file("file_13_dir1.txt");
    create_file("file_14_dir1.txt");
    create_file("file_15_dir1.txt");
    create_file("file_16_dir1.txt");
    create_file("file_17_dir1.txt");
    create_file("file_18_dir1.txt");
    ls_dir();
    printf("********************\n");

    change_dir("dir 1_1");
    printf("\n********************\ncurrent dir: dir 1_1\n\n");
    create_file("file_1_dir1_1.txt");
    create_file("file_2_dir1_1.txt");
    create_file("file_3_dir1_1.txt");
    create_file("file_4_dir1_1.txt");
    create_file("file_5_dir1_1.txt");
    create_file("file_6_dir1_1.txt");
    create_file("file_7_dir1_1.txt");
    create_file("file_8_dir1_1.txt");
    create_dir("test");
    ls_dir();
    printf("********************\n");

    change_dir("test");
    printf("\n********************\ncurrent dir: test\n\n");
    create_file("test1.txt");
    create_file("test2.txt");
    ls_dir();
    printf("********************\n");

    change_dir("..");
    printf("\n********************\ncurrent dir: dir 1_1\n\n");
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

    return 0;
}