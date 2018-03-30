#include "commands.h"
#include "log.h"
#include <cassert>
#include <cstring>

#define TAG "CMD"

/** String repr of command type */
static const char* StringMap[] = {
    "BUFSAVE",
    "BUFOPEN",
};


/** Command definitions corresponding to cmd type */
static const char* Defs[] = {
    "w",    // BUFSAVE
    "e",    // BUFOPEN
};

Command::Command()
    : valid(false), type(Type::Unknown)
{
    for (unsigned int i = 0; i < 16; ++i) {
        cmd[i] = '\0';
    }
    for (unsigned int i = 0; i < 128; ++i) {
        args[i] = '\0';
    }
}

Command::Command(const char* cmdstr)
    : Command()
{
    assert(cmdstr);
    parseArgs(cmdstr);
    idType();
}


void Command::parseArgs(const char* cmdstr)
{
    unsigned int len = strlen(cmdstr);
    unsigned int i   = 0;
    // cmd
    for (i = 0; i < len; ++i) {
        cmd[i] = cmdstr[i];
        if (cmdstr[i] == ' ') {
            cmd[i] = '\0';
            break;
        }
    }

    // Skip space
    unsigned int space = 0;    // Whitespace b/w cmd and arg
    unsigned int k;
    for (k = i; k < len; ++k) {
        if (cmdstr[k] != ' ') {
            break;
        } else {
            space++;
        }
    }

    // args
    unsigned int j;
    for (j = 0; k < len; ++k, ++j) {
        args[j] = cmdstr[k];
    }
    args[j + 1] = '\0';
}

void Command::idType()
{
    int count = 0;
    for (auto& def : Defs) {
        if (strcmp(cmd, def) == 0) {
            log_l(TAG, "Recognized command: %s", cmd);
            valid = true;
            type  = (Type) count;
            break;
        }
        count++;
    }
}

void Command::pprint() const
{
    log_l(TAG, "Command {valid: %d, cmd: %s, args: %s}",
          valid, cmd, args);
}
