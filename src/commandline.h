#pragma once
#include "common.h"
#define CMDLINE_SIZE 256

/** Each command has a unique id which corresponds to cmd_str[] */
enum cmd_type {
    CMD_BUFSAVE = 0,
    CMD_BUFOPEN,

    CMD_ENUMCOUNT,    // Number of commands (last element)
};


/** String repr of command_type */
static const char* cmd_str[] = {
    "BUFSAVE",
    "BUFOPEN",
};


/** Command definitions corresponding to cmd_type */
static const char* cmd_defs[] = {
    "w",    // BUFSAVE
    "e",    // BUFOPEN
};


union cmd_data {
    int i;        // Integer/count
    char* str;    // String
    int b;        // Bool
};


struct command {
    int valid;    // 0 if invalid
    enum cmd_type type;
    union cmd_data data;
    const char* cmdstr;    // original command string
    char cmd[16];
    char args[128];
};


/* Commandline is just a buffer for now */
struct cmdline {
    size_t cur;
    char buffer[CMDLINE_SIZE];
};


struct cmdline cmd_create(void);

/** Add ch to cmdline */
void cmd_addch(struct cmdline* cl, char ch);
void cmd_delch(struct cmdline* cl);
void cmd_clear(struct cmdline* cl);
struct command cmd_parse(struct cmdline* cl);

/** Parse command string and return command */
struct command cmd_parse_string(const char* cmdstr);

void cmd_pprint(const struct command cmd);
