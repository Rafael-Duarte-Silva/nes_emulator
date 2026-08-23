#include "log.h"
#include <string.h>

#ifdef __EMSCRIPTEN
#define NO_ANSI
#endif

static char *type[LOG_LEVEL_LEN] = {
    "DEBUG",
    "SUCCESS",
    "WARN",
    "ERR"};

static char *colors[LOG_LEVEL_LEN] = {
    "\x1b[90m",
    "\x1b[32m",
    "\x1b[1;33m",
    "\x1b[31m"};

#define RESET_COLOR "\x1b[0m"

static time_t current_time;
static struct tm *m_time;

static void get_time()
{
    time(&current_time);
    m_time = localtime(&current_time);
}

void ulogger_log(LOG_LEVEL level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    get_time();
#ifndef NO_ANSI
    printf("%s", colors[level]);
#endif

    printf("[%d/%d/%d -> %d:%d:%d][%s] ", m_time->tm_mday,
           m_time->tm_mon + 1,
           m_time->tm_year + 1900,
           m_time->tm_hour,
           m_time->tm_min,
           m_time->tm_sec,
           type[level]);
    vfprintf(stdout, fmt, args);
    printf("\n%s", RESET_COLOR);
    va_end(args);
}