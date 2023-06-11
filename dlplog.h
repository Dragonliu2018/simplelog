/*
 * @Author: 刘振龙 dragonliu@buaa.edu.cn
 * @Date: 2023-06-08 18:01:53
 * @LastEditors: 刘振龙 dragonliu@buaa.edu.cn
 * @LastEditTime: 2023-06-11 11:51:53
 * @FilePath: /dlplog/dlplog.h
 * @Description: the header file of dlplog
 */

#ifndef __DLPLOG_H__
#define __DLPLOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils/common.h"
#include "utils/loginit.h"
#include "utils/parsejson.h"

// 配置文件指针
LogConfig *g_dlplog_config = NULL;

// 日志文件指针
LogFile *g_dlplog_log_file = NULL;

extern const char *g_dlplog_level_str_arr[];
extern const char *g_dlplog_submodule_name_str_arr[];

// 初始化
// static inline 调研
void log_init()
{
    if (g_dlplog_config == NULL) {
        printf("log init...\n\n");
        // 解析配置文件
        parse_json_file(LOG_CONFIG_PATH, &g_dlplog_config);
        // 初始化目录
        init_log_dir(g_dlplog_config); 
        // 初始化文件指针哈希表
        init_file_ptr_hash(g_dlplog_config, &g_dlplog_log_file);
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
    char *timestamp = (char *)malloc(MAX_TIMESTAMP_LEN);
    if (timestamp == NULL) {
        printf("timestamp's memory allocation failed!\n");
        return;
    }
    memset(timestamp, 0, strlen(timestamp));
    get_timestamp(timestamp);

    // 子模块名称
    const char *submoduleString = g_dlplog_submodule_name_str_arr[submodule];

    // 日志等级
    const char *levelString = g_dlplog_level_str_arr[level];

    // 进程号
    pid_t pid = getpid();

    // 输出日志消息
    LogFile *logFile = NULL;
    assert(g_dlplog_log_file != NULL);
    HASH_FIND_STR(g_dlplog_log_file, submoduleString, logFile);
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

#endif /* __DLPLOG_H__ */
