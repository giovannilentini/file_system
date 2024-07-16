#include <stdio.h>

#include "../include/file_system.h"

int main() {
    init("bin/test_disk.img");

    char buf[2048];

    printf("\n********************\ncurrent dir: root\n");
    create_file("read_test.txt");
    copy_to_my_fs("test/read_test.txt", "read_test.txt");
    read_file("read_test.txt", buf, 2048);

    printf("\n\n%s\n\n", buf);
    printf("********************\n");

    printf("\n********************\ncurrent dir: root\n");
    create_file("write_test.txt");
    write_file("write_test.txt", "Ciaooooooooooooooo Scrivo\nTest\nAh Boh", 2048);
    copy_from_my_fs("write_test.txt", "test/write_test.txt");
    printf("********************\n");
}