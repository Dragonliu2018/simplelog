/*
 * @Author: 刘振龙 dragonliu@buaa.edu.cn
 * @Date: 2023-06-08 18:01:53
 * @LastEditors: 刘振龙 dragonliu@buaa.edu.cn
 * @LastEditTime: 2023-06-10 20:07:17
 * @FilePath: /dlplog/dlplog.h
 * @Description: the header file of dlplog
 */

#ifndef __DLPLOG_H__
#define __DLPLOG_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "utils/common.h"
#include "utils/parsejson.h"
#include "utils/loginit.h"

// 配置文件
const char *logConfigPath = "./conf/logconf.json"; // 宏定义
LogConfig config; // g_dlplog_config 定义成指针 判空

// 日志文件指针
LogFile *logFileHash; // 全局变量命名前面增加g_dlplog_

const char *level_str_arr[10]; 
#define STRINGIFY(x) #x

// 初始化
// static inline 调研
void log_init() {
    static int isFirstCall = 1; // 1改成宏定义

    if (isFirstCall == 1) {
        printf("log init...\n\n");
        // 解析配置文件
        parse_json_file(logConfigPath, &config);
        // 初始化目录
        init_log_dir(&config); 
        // 初始化文件指针哈希表
        init_file_ptr_hash(&config, &logFileHash);

        // 初始化level全局
        for (int i = 0; i < MAX_LEVEL_NUM; i++) { // for抽离函数
            level_str_arr[i] = (const char *)malloc(10);
            level_str_arr[i] = STRINGIFY(LogLevel(i));
        }
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

#endif /* __DLPLOG_H__ */
