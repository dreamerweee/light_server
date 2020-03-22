#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#define LOG_TYPE_DEBUG  0
#define LOG_TYPE_INFO   1
#define LOG_TYPE_WARN   2
#define LOG_TYPE_ERROR  3

void LogDebug(const char *fmt, ...);
void LogInfo(const char *fmt, ...);
void LogWarn(const char *fmt, ...);
void LogError(const char *fmt, ...);

#endif  // LOG_H