#include <stdio.h>

#include "../include/file_system.h" 

int main() {
    init("bin/test_disk.img");
    char buf[5] = {0};
    char buf2[40] = {0};
    char buf3[6] = {0};

    printf("\n********************\ncurrent dir: root\n\n");
    write_file("file_1_root.txt", "ciao", 4);
    read_file("file_1_root.txt", buf, 4);
    printf("read from file '/file_1_root.txt' = %s\n", buf);

    write_file("file_2_root.txt", "ciao", 4);
    read_file("file_2_root.txt", buf, 4);
    printf("read from file '/file_2_root.txt' = %s\n", buf);

    write_file("file_3_root.txt", "nuovo", 5);
    read_file("file_3_root.txt", buf3, 5);
    printf("read from file '/file_3_root.txt' = %s\n", buf3);

    write_file("file_2_root.txt", "nuovo", 5);
    read_file("file_2_root.txt", buf3, 5);
    printf("read from file '/file_2_root.txt' = %s\n", buf3);
    
    write_file("file_1_root.txt", "gggggg", 6);
    read_file("file_1_root.txt", buf2, 40);
    printf("read from file '/file_1_root.txt' = %s\n", buf2);
    printf("********************\n");

    change_dir("dir 1");
    printf("\n********************\ncurrent dir: dir 1\n\n");
    write_file("file_1_dir1.txt", "ciao 2", 6);
    read_file("file_1_dir1.txt", buf2, 6);
    printf("read from file '/dir 1/file_1_dir 1.txt' = %s\n", buf2);
    printf("********************\n");

    printf("\n********************\nother test on change_dir\n\n");
    write_file("file_1_root.txt", "ciao", 4);
    write_file("file_1_dir1_1.txt", "ciao", 4);
    printf("********************\n\n");

    return 0;
}