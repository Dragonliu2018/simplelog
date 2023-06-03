#pragma once

#include "utils/common.h"
#include "utils/parsejson.h"
#include "utils/loginit.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// 配置文件
const char *logConfigPath = "./conf/logconf.json";
LogConfig config;

// 日志文件指针
LogFile *logFileHash;

// 初始化
void log_init() {
    static int isFirstCall = 1;

    if (isFirstCall) {
        printf("log init...\n\n");
        // 解析配置文件
        parse_json_file(logConfigPath, &config);
        // 初始化目录
        init_log_dir(&config); 
        // 初始化文件指针哈希表
        init_file_ptr_hash(&config, &logFileHash);

        isFirstCall = 0;
    }
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
    LogFile *logFile = NULL;
    assert(logFileHash != NULL);
    HASH_FIND_STR(logFileHash, submoduleString, logFile);
    assert(logFile != NULL);
    FILE *file = logFile->file;

    fprintf(file, "[%s] [%s] [%s] process_id=\"p%d\" ", timestamp, submoduleString, levelString, pid);
    fprintf(file, "event_message=");
    // 处理可变参数和格式化字符串
    va_list args;
    va_start(args, fmt);
    fprintf(file, "\"");
    vfprintf(file, fmt, args);
    fprintf(file, "\" ");
    va_end(args);

    fprintf(file, "file_name=\"%s\" func_name=\"%s\" file_line=\"%d\" ", fileName, funcName, line);
    if (level == ERROR) {
        fprintf(file, "stack_trace=\n");
        print_call_stack(file);
    }
    fprintf(file, "\n\n");
}

/* 外部接口 */
#define LOG_DEBUG(submodule, fmt, ...) LOG(submodule, DEBUG, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(submodule, fmt, ...) LOG(submodule, INFO, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARNIGN(submodule, fmt, ...) LOG(submodule, WARNING, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(submodule, fmt, ...) LOG(submodule, ERROR, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
