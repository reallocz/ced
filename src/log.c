#include "log.h"
#include "fileutils.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_LOGFILE "log.txt"

static struct {
    const char* logfilename;
    FILE* logfile;
} G;


void log_init(void)
{
    G.logfilename = LOG_LOGFILE;
    G.logfile     = fopen(G.logfilename, "w");
    assert(G.logfile);
    fprintf(G.logfile, "--LOG BEGIN--\n");
}


void log_exit(void)
{
    if (G.logfile != NULL) {
        fprintf(G.logfile, "\n--LOG END--\n");
        fflush(G.logfile);
        fclose(G.logfile);
    }
    printf("closing log file\n");
}


void log_l(const char* tag, const char* msg, ...)
{
    fprintf(G.logfile, "[%s] ", tag);
    va_list args;
    va_start(args, msg);
    vfprintf(G.logfile, msg, args);
    va_end(args);
    fprintf(G.logfile, "\n");
    fflush(G.logfile);
}


void log_lc(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    vfprintf(G.logfile, msg, args);
    va_end(args);
    fflush(G.logfile);
}


void log_e(const char* tag, const char* msg, ...)
{
    fprintf(G.logfile, "**[%s] ", tag);
    va_list args;
    va_start(args, msg);
    vfprintf(G.logfile, msg, args);
    va_end(args);
    fprintf(G.logfile, "\n");
    fflush(G.logfile);
}


void log_ec(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    vfprintf(G.logfile, msg, args);
    va_end(args);
    fflush(G.logfile);
}

void log_fatal(const char* tag, const char* msg, ...)
{
    fprintf(G.logfile, "----FATAL-----\n");
    fprintf(G.logfile, "**[%s]** ", tag);
    va_list args;
    va_start(args, msg);
    vfprintf(G.logfile, msg, args);
    va_end(args);
    fprintf(G.logfile, "\n");
    fflush(G.logfile);
    log_exit();
    exit(1);
}
