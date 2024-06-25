#include <stdio.h>
#include <string.h>

#include "../include/file_system.h"

int main() {
    init("bin/test_disk.img");

    char buf[5] = {0}; 
    char buf2[80];

    printf("\n********************\ncurrent dir: root\n\n");
    write_file("file_2_root.txt", "ciao", 4);
    read_file("file_2_root.txt", buf, 4);
    printf("read from file '/file_2_root.txt' = %s\n", buf);
    seek("file_2_root.txt", 4);
    write_file("file_2_root.txt", " riscrivo", strlen(" riscrivo"));
    seek("file_2_root.txt", 5);
    read_file("file_2_root.txt", buf2, 80);
    printf("read from file '/file_2_root.txt' after seeking to position 4 for writing and seeking to position 5 for reading = %s\n", buf2);

    seek("file_2_root.txt", 0);
    read_file("file_2_root.txt", buf2, 80);
    printf("read from file '/file_2_root.txt' after seeking to position 0 for reading = %s\n", buf2);
    printf("********************\n\n");

    return 0;
}