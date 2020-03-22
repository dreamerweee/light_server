#include "log.h"

// 输出日志
static void Logger(int log_type, const char *buffer)
{
    const char *type_msg;
    switch (log_type) {
        case LOG_TYPE_DEBUG:
            type_msg = "debug";
            break;
        case LOG_TYPE_INFO:
            type_msg = "info";
            break;
        case LOG_TYPE_WARN:
            type_msg = "warn";
            break;
        case LOG_TYPE_ERROR:
            type_msg = "error";
            break;
        default :
            type_msg = "unknow";
            break;
    }
    fprintf(stdout, "[%s] %s\n", type_msg, buffer);
}

// 处理需要输出的日志
static void LogHandler(int log_type, const char *fmt, va_list args, int err_flag = 0)
{
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    if (err_flag == 1) {
        int len = strlen(buffer);
        const char *err_str = strerror(errno);
        if (len + strlen(err_str) < sizeof(buffer) - 3) {
            snprintf(buffer + len, sizeof(buffer) - len, ": %s", err_str);
        }
    }
    Logger(log_type, buffer);
}

// 调试信息
void LogDebug(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LogHandler(LOG_TYPE_DEBUG, fmt, args);
    va_end(args);
}

// 一般信息
void LogInfo(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LogHandler(LOG_TYPE_INFO, fmt, args);
    va_end(args);
}

// 警告
void LogWarn(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LogHandler(LOG_TYPE_WARN, fmt, args);
    va_end(args);
}

// 错误
void LogError(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LogHandler(LOG_TYPE_ERROR, fmt, args, 1);
    va_end(args);
}
