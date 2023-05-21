#include <stdio.h>
#include <time.h>

// 日志等级
typedef enum {
    DEBUG,
    INFO,
    WARNING,
    ERROR
} LogLevel;

// 获得当前时间
const char *get_timestamp()
{
    time_t t = time(NULL);
    struct tm* currentTime = localtime(&t);
    static char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", currentTime);
    return timestamp;
}

// 通过LogLevel获得字符串
const char *get_log_level_str(LogLevel level)
{
    // 根据日志级别选择输出格式
    const char* levelString;

    switch (level) {
        case DEBUG:
            levelString = "DEBUG";
            break;
        case INFO:
            levelString = "INFO";
            break;
        case WARNING:
            levelString = "WARNING";
            break;
        case ERROR:
            levelString = "ERROR";
            break;
        default:
            levelString = "UNKNOWN";
            break;
    }

    return levelString;
}

// 日志输出
void LOG(LogLevel level, const char *message)
{
    // 获取当前时间
    const char *timestamp = get_timestamp();

    // 日志等级
    const char *levelString = get_log_level_str(level);

    // 输出日志消息
    printf("[%s] [%s] %s\n", timestamp, levelString, message);
}

/* 外部接口 */
void LOG_DEBUG(const char *message)
{
    LOG(DEBUG, message);
}

void LOG_INFO(const char *message)
{
    LOG(INFO, message);
}

void LOG_WARNIGN(const char *message)
{
    LOG(WARNING, message);
}

void LOG_ERROR(const char *message)
{
    LOG(ERROR, message);
}