#include "commandline.h"
#include "log.h"
#include <assert.h>
#include <string.h>


#define TAG "CMD"

struct cmdline cmd_create(void)
{
    struct cmdline cl;
    cmd_clear(&cl);
    return cl;
}

void cmd_addch(struct cmdline* cl, char ch)
{
    assert(cl);
    if(cl->cur < CMDLINE_SIZE) {
        cl->buffer[cl->cur++] = ch;
        cl->buffer[cl->cur] = '\0';
    } else {
        log_l(TAG, "OUT OF BOUNDS!!");
    }
}

void cmd_delch(struct cmdline* cl)
{
    assert(cl);
    if(cl->cur > 0) {
        cl->buffer[--cl->cur]= '\0';
    } else {
        cmd_clear(cl);
    }
}

void cmd_clear(struct cmdline* cl)
{
    assert(cl);
    cl->cur = 0;
    cl->buffer[cl->cur] = '\0';
}

struct command cmd_parse(struct cmdline* cl){
    return cmd_parse_string(cl->buffer);
}


struct command cmd_parse_string(const char* cmdstr)
{
    assert(cmdstr);
    struct command c = {.valid = 0, .cmdstr = cmdstr};

    // Parse string into cmd and args
    {
        size_t len = strlen(cmdstr);
        size_t i   = 0;
        // cmd
        for (i = 0; i < len; ++i) {
            c.cmd[i] = cmdstr[i];
            if (cmdstr[i] == ' ') {
                c.cmd[i] = '\0';
                break;
            }
        }

        // Skip space
        size_t space = 0;    // Whitespace b/w cmd and arg
        size_t k;
        for (k = i; k < len; ++k) {
            if (cmdstr[k] != ' ') {
                break;
            } else {
                space++;
            }
        }

        // args
        size_t j;
        for (j = 0; k < len; ++k, ++j) {
            c.args[j] = cmdstr[k];
        }
        c.args[j + 1] = '\0';
    }


    // Identify command
    for (size_t i = 0; i < CMD_ENUMCOUNT; ++i) {
        if (strcmp(c.cmd, cmd_defs[i]) == 0) {
            log_l(TAG, "Recognized command: %s", c.cmd);
            c.valid = 1;
            c.type  = i;
            break;
        }
    }
    return c;
}


void cmd_pprint(const struct command cmd)
{
    log_l(TAG, "Command {valid: %d, cmd: %s, args: %s}", cmd.valid,
          cmd.cmd, cmd.args);
}
