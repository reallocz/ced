#include "log.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "fileutils.h"

#define LOG_LOGFILE "log.txt"

static struct {
	const char* logfilename;
	FILE* logfile;
} G;


void log_init(void)
{
	G.logfilename = LOG_LOGFILE;
        G.logfile = fopen(G.logfilename, "w");
        assert(G.logfile);
        fprintf(G.logfile, "--BEGIN--\n");
}


void log_exit(void)
{
        if(G.logfile != NULL) {
                fprintf(G.logfile, "\n--END--\n");
                fclose(G.logfile);
        }
        printf("closing log file");
}


void log_l(const char* tag, const char* msg, ...)
{
        fprintf(G.logfile, "[%s]", tag);
        va_list args;
        va_start(args, msg);
        vfprintf(G.logfile, msg, args);
        va_end(args);
        fprintf(G.logfile, "\n");
}

void log_e(const char* tag, const char* msg, ...)
{
        fprintf(G.logfile, "**[%s]", tag);
        va_list args;
        va_start(args, msg);
        vfprintf(G.logfile, msg, args);
        va_end(args);
        fprintf(G.logfile, "\n");
}
