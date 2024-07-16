#pragma once

#define MAX_COMMAND_LEN 512
#define MAX_ARGC 2

void format(int argc, char* argv[MAX_ARGC + 1]);

void touch(int argc, char* argv[MAX_ARGC + 1]);

void mkdir(int argc, char* argv[MAX_ARGC + 1]);

void rm(int argc, char* argv[MAX_ARGC + 1]);

void rmrf(int argc, char* argv[MAX_ARGC + 1]);

void ls(int argc, char* argv[MAX_ARGC + 1]);

void cd(int argc, char* argv[MAX_ARGC + 1]);

void my_write(int argc, char* argv[MAX_ARGC + 1]);

void my_read(int argc, char* argv[MAX_ARGC + 1]);

void copy_from_host(int argc, char* argv[MAX_ARGC + 1]);

void copy_to_host(int argc, char* argv[MAX_ARGC + 1]);

void clear(int argc, char* argv[MAX_ARGC + 1]);

void do_command();