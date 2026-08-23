#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdbool.h>

typedef enum
{
    LOG_DBG,
    LOG_SUCCESS,
    LOG_WARN,
    LOG_ERR,
    LOG_LEVEL_LEN
} LOG_LEVEL;

void set_log_config(const char enable_argument[]);
void ulogger_log(LOG_LEVEL level, const char *fmt, ...);

#define LOG_DEBUG(...) ulogger_log(LOG_DBG, __VA_ARGS__)
#define LOG_SUCCESS(...) ulogger_log(LOG_SUCCESS, __VA_ARGS__)
#define LOG_WARN(...) ulogger_log(LOG_WARN, __VA_ARGS__)
#define LOG_ERR(...) ulogger_log(LOG_ERR, __VA_ARGS__)

#endif