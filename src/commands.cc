#include "commands.h"
#include "log.h"
#include <assert.h>
#include <string.h>


#define TAG "CMD"


struct command cmd_parse_string(const char* cmdstr)
{
    assert(cmdstr);
    struct command c;
    c.valid  = 0;
    c.cmdstr = cmdstr;

    // Parse string into cmd and args
    {
        unsigned int len = strlen(cmdstr);
        unsigned int i   = 0;
        // cmd
        for (i = 0; i < len; ++i) {
            c.cmd[i] = cmdstr[i];
            if (cmdstr[i] == ' ') {
                c.cmd[i] = '\0';
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
            c.args[j] = cmdstr[k];
        }
        c.args[j + 1] = '\0';
    }


    // Identify command
    for (unsigned int i = 0; i < CMD_ENUMCOUNT; ++i) {
        if (strcmp(c.cmd, cmd_defs[i]) == 0) {
            log_l(TAG, "Recognized command: %s", c.cmd);
            c.valid = 1;
            //c.type  = i;
            break;
        }
    }
    return c;
}


void cmd_pprint(const struct command cmd)
{
    log_l(TAG, "Command {valid: %d, cmd: %s, args: %s}",
          cmd.valid, cmd.cmd, cmd.args);
}
