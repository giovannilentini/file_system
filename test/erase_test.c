#include <stdio.h>

#include "../include/file_system.h"

int main() {
    init("bin/test_disk.img");

    printf("\n********************\ncurrent dir: root\n\n");
    create_dir("erase_test_dir");
    erase_file("dir 10");
    change_dir("dir 11");
    create_file("file_1_dir_11.txt");
    change_dir("..");
    erase_file("dir 11");
    change_dir("dir 11");
    erase_file("file_1_dir_11.txt");
    change_dir("..");
    erase_file("dir 11");
    ls_dir();
    printf("********************\n");

    change_dir("erase_test_dir");
    printf("\n********************\ncurrent dir: erase_test_dir\n\n");
    create_file("file_to_delete.txt");
    write_file("file_to_delete.txt", "ciao", 4);
    create_dir("empty_dir");
    create_dir("full_dir");
    ls_dir();
    printf("********************\n");

    change_dir("full_dir");
    printf("\n********************\ncurrent dir: full_dir\n\n");
    create_file("1.txt");
    create_file("2.txt");
    create_file("3.txt");
    create_dir("empty_dir_2"); 
    create_dir("full_dir_2");
    ls_dir();
    printf("********************\n");

    change_dir("full_dir_2");
    create_file("11.txt");
    create_file("22.txt");
    create_dir("ciao");
    change_dir("ciao");
    create_file("reqr");
    change_dir("..");

    change_dir("..");
    change_dir("..");
    printf("\n********************\ncurrent dir: erase_test_dir\n\n");
    printf("ls before delete 'file_to_delete.txt'\n\n");
    ls_dir();
    erase_file("file_to_delete.txt");
    printf("\nls after delete 'file_to_delete.txt'\n\n");
    ls_dir();

    printf("\nls before delete 'empty_dir'\n\n");
    ls_dir();
    erase_file("empty_dir");
    printf("\nls after delete 'empty_dir'\n\n");
    ls_dir();

    printf("\nls before delete 'full_dir'\n\n");
    ls_dir();
    erase_dir_recursive("full_dir");
    printf("\nls after delete 'full_dir'\n\n");
    ls_dir();
    printf("********************\n\n");

    change_dir("full_dir");
    change_dir("full_dir_2");
    ls_dir();

    return 0;
}