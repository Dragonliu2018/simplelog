#include <stdio.h>
#include <time.h>

// 日志等级
typedef enum {
    DEBUG,
    INFO,
    WARNING,
    ERROR
} LogLevel;

// TODO: 补充子模块名称
// 子模块名称
typedef enum {
    Sampling_Node, // 采集节点
    Analysis_Node, // 分析节点
    Management_Node, // 管理节点
} SubmoduleName;

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
            levelString = "UNKNOWN LogLevel";
            break;
    }

    return levelString;
}

// 通过SubmoduleName获得字符串
const char *get_submodule_name_str(SubmoduleName name)
{
    // 根据子模块选择输出格式
    const char* nameString;

    switch (name) {
        case Sampling_Node:
            nameString = "Sampling Node";
            break;
        case Analysis_Node:
            nameString = "Analysis Node";
            break;
        case Management_Node:
            nameString = "Management Node";
            break;
        default:
            nameString = "UNKNOWN SubmoduleName";
            break;
    }

    return nameString;
}

// 日志输出
void LOG(SubmoduleName submodule, LogLevel level, const char *message)
{
    // 获取当前时间
    const char *timestamp = get_timestamp();

    // 子模块名称
    const char *submoduleString = get_submodule_name_str(submodule);

    // 日志等级
    const char *levelString = get_log_level_str(level);

    // 输出日志消息
    printf("[%s] [%s] [%s] %s\n", timestamp, submoduleString, levelString, message);
}

/* 外部接口 */
void LOG_DEBUG(SubmoduleName name, const char *message)
{
    LOG(name, DEBUG, message);
}

void LOG_INFO(SubmoduleName name, const char *message)
{
    LOG(name, INFO, message);
}

void LOG_WARNIGN(SubmoduleName name, const char *message)
{
    LOG(name, WARNING, message);
}

void LOG_ERROR(SubmoduleName name, const char *message)
{
    LOG(name, ERROR, message);
}