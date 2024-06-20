#include <stdio.h>
#include <string.h>

#include "../include/file_system.h"

int main() {
    init("bin/test_disk.img");

    int l = strlen("ciao") + strlen(" riscrivo");
    int x = l+1;

    char buf[5] = {0}; 
    char buf2[x];

    printf("\n********************\ncurrent dir: root\n\n");
    write_file("file_2_root.txt", "ciao", 4);
    read_file("file_2_root.txt", buf, 4);
    printf("read from file '/file_2_root.txt' = %s\n", buf);
    seek("file_2_root.txt", 4);
    write_file("file_2_root.txt", " riscrivo", strlen(" riscrivo"));
    seek("file_2_root.txt", 0);
    read_file("file_2_root.txt", buf2, l-1);
    printf("read from file '/file_2_root.txt' after seek = %s\n", buf2);
    printf("********************\n\n");
}