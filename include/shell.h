#pragma once

#define MAX_COMMAND_LEN 512
#define MAX_ARGC 3

void format(int argc, char* argv[MAX_ARGC + 1]);

void touch(int argc, char* argv[MAX_ARGC + 1]);

void mkdir(int argc, char* argv[MAX_ARGC + 1]);

void rm(int argc, char* argv[MAX_ARGC + 1]);

void rmrf(int argc, char* argv[MAX_ARGC + 1]);

void ls(int argc, char* argv[MAX_ARGC + 1]);

void cd(int argc, char* argv[MAX_ARGC + 1]);

void write(int argc, char* argv[MAX_ARGC + 1]);

void read(int argc, char* argv[MAX_ARGC + 1]);

void clear(int argc, char* argv[MAX_ARGC + 1]);

void do_command();