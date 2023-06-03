#pragma once

#include "parsejson.h"

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <execinfo.h>
#include <stdarg.h>

// 配置文件
const char *logConfigPath = "logconf.json";
LogConfig config;

// 初始化
void log_init() {
    static int isFirstCall = 1;

    if (isFirstCall) {
        printf("log init...\n\n");
        parse_json_file(logConfigPath, &config);
        isFirstCall = 0;
    }
}

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
            nameString = "Sampling_Node";
            break;
        case Analysis_Node:
            nameString = "Analysis_Node";
            break;
        case Management_Node:
            nameString = "Management_Node";
            break;
        default:
            nameString = "UNKNOWN_SubmoduleName";
            break;
    }

    return nameString;
}

// 获得调用栈信息
void print_call_stack() 
{
    void* call_stack[50];
    int stack_depth = backtrace(call_stack, 50);
    char** stack_symbols = backtrace_symbols(call_stack, stack_depth);

    if (stack_symbols == NULL) {
        printf("无法获取调用栈信息\n");
        return;
    }

    for (int i = 0; i < stack_depth; i++) {
        printf("%s\n", stack_symbols[i]);
    }

    free(stack_symbols);
}

// 日志输出
void LOG(SubmoduleName submodule,
         LogLevel level,
         const char *fileName,
         const char *funcName,
         int line,
         const char* fmt,
         ...)
{
    // 日志系统初始化，只被调用一次
    log_init();

    // 获取当前时间
    const char *timestamp = get_timestamp();

    // 子模块名称
    const char *submoduleString = get_submodule_name_str(submodule);

    // 日志等级
    const char *levelString = get_log_level_str(level);
    
    // 进程号
    pid_t pid = getpid();

    // 输出日志消息
    printf("[%s] [%s] [%s] process_id=\"p%d\" ", timestamp, submoduleString, levelString, pid);
    printf("event_message=");
    // 处理可变参数和格式化字符串
    va_list args;
    va_start(args, fmt);
    printf("\"");
    vprintf(fmt, args);
    printf("\" ");
    va_end(args);

    printf("file_name=\"%s\" func_name=\"%s\" file_line=\"%d\" ", fileName, funcName, line);
    if (level == ERROR) {
        printf("stack_trace=\n");
        print_call_stack();
    }
    printf("\n\n");
}

/* 外部接口 */
#define LOG_DEBUG(submodule, fmt, ...) LOG(submodule, DEBUG, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(submodule, fmt, ...) LOG(submodule, INFO, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARNIGN(submodule, fmt, ...) LOG(submodule, WARNING, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(submodule, fmt, ...) LOG(submodule, ERROR, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
