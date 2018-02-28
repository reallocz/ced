#pragma once

/** Initialize the log file */
void log_init(void);

/** Flush and close the log file */
void log_exit(void);

/** Standard log lvl */
void log_l(const char* tag, const char* msg, ...);

/** Error log lvl */
void log_e(const char* tag, const char* msg, ...);

