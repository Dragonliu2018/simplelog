/*
 * @Author: 刘振龙 dragonliu@buaa.edu.cn
 * @Date: 2023-06-08 18:01:53
 * @LastEditors: 刘振龙 dragonliu@buaa.edu.cn
 * @LastEditTime: 2023-06-11 10:29:59
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

#define STRINGIFY(x) #x
#define LOG_CONFIG_PATH "./conf/logconf.json"

// 日志等级对应的字符串
const char *g_dlplog_level_str_arr[10];

// 配置文件指针
LogConfig *g_dlplog_config = NULL;

// 日志文件指针
LogFile *g_dlplog_log_file = NULL;

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

        // 初始化level全局
        for (int i = 0; i < MAX_LEVEL_NUM; i++) { // for抽离函数
            g_dlplog_level_str_arr[i] = (const char *)malloc(10);
            if (g_dlplog_level_str_arr[i] == NULL) {
                printf("g_dlplog_level_str_arr[%d]'s memory allocation failed!\n", i);
                return;
            }
            memset((void *)g_dlplog_level_str_arr[i], 0, strlen(g_dlplog_level_str_arr[i]));
            g_dlplog_level_str_arr[i] = STRINGIFY(LogLevel(i));
        }
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
    // 分配栈空间，传指针
    const char *timestamp = get_timestamp();

    // 子模块名称
    const char *submoduleString = get_submodule_name_str(submodule);

    // 日志等级
    const char *levelString = get_log_level_str(level);

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
