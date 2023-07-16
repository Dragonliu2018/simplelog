/*
 * @Author: 刘振龙 dragonliu@buaa.edu.cn
 * @Date: 2023-06-08 18:01:53
 * @LastEditors: 刘振龙 dragonliu@buaa.edu.cn
 * @LastEditTime: 2023-07-16 13:22:06
 * @FilePath: /dlplog/dlplog.h
 * @Description: the header file of dlplog
 */

#ifndef __DLPLOG_H__
#define __DLPLOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils/common.h"
#include "utils/logfilehandle.h"

// 日志文件信息数组
LogFile *g_dlplog_log_file_hash = NULL;

extern const char *g_dlplog_level_str_arr[];

// 初始化
static inline bool log_init()
{
    if (g_dlplog_log_file_hash == NULL) {
        printf("log init...\n\n");
        // 解析配置文件
        parse_json_file(LOG_CONFIG_PATH, &g_dlplog_log_file_hash);
        if (g_dlplog_log_file_hash == NULL) {
            printf("Error: parse_json_file failed!\n");
            return false;
        }
    }
    return true;
}

// 日志输出
static inline void LOG(const char *submodule,
                       LogLevel level,
                       const char *fileName,
                       const char *funcName,
                       int line,
                       const char* fmt,
                       ...)
{
    // 日志系统初始化，只被调用一次
    if (log_init() == false) {
        printf("Error: log init failed!\n");
        return;
    }

    // 处理log file
    LogFile* logFile;
    HASH_FIND_STR(g_dlplog_log_file_hash, submodule, logFile);
    if (logFile == NULL)
        add_log_file(submodule);
    else
        update_log_file(logFile);

    // 判断logging_enable
    if (strcmp(logFile->logging_enable, "off") == 0) {
        return;
    } else if (strcmp(logFile->logging_enable, "on") != 0) {
        printf("Error: LogConfig's logging_enable is only 'on' or 'off', cannot %s!\n", logFile->logging_enable);
        return;
    }
    // 判断log_min_messages
    if (level < string2LogLevel(logFile->log_min_messages)) {
        return;
    }

    // 获取当前时间
    char timestamp[MAX_TIMESTAMP_LEN];
    get_timestamp(timestamp);

    // 日志等级
    const char *levelString = g_dlplog_level_str_arr[level];

    // 进程号
    pid_t pid = getpid();

    // 输出日志消息
    FILE *file = logFile->file;
    if (file == NULL) {
        printf("Error: g_dlplog_log_file_hash[%s] is NULL!\n", submodule);
        return;
    }

    fprintf(file, "[%s] [%s] [%s] process_id=p%d ", timestamp, submodule, levelString, pid);
    fprintf(file, "event_message=");
    // 处理可变参数和格式化字符串
    va_list args;
    va_start(args, fmt);
    fprintf(file, "\"");
    vfprintf(file, fmt, args);
    fprintf(file, "\" ");
    va_end(args);

    fprintf(file, "file_name=%s func_name=%s file_line=%d ", fileName, funcName, line);
    if (level == ERROR) {
        fprintf(file, "stack_trace=\n");
        print_call_stack(file);
    }
    fprintf(file, "\n");

    // 立即刷新文件缓冲区
    fflush(file);  
}

/* 外部接口 */
#define LOG_DEBUG(submodule, fmt, ...) LOG(submodule, DEBUG, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(submodule, fmt, ...) LOG(submodule, INFO, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARNIGN(submodule, fmt, ...) LOG(submodule, WARNING, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(submodule, fmt, ...) LOG(submodule, ERROR, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

#endif /* __DLPLOG_H__ */
