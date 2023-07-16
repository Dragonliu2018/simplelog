/*
 * @Author: 刘振龙 dragonliu@buaa.edu.cn
 * @Date: 2023-06-08 18:01:53
 * @LastEditors: 刘振龙 dragonliu@buaa.edu.cn
 * @LastEditTime: 2023-07-16 13:19:50
 * @FilePath: /dlplog/utils/common.h
 * @Description: common parts of dlplog
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <execinfo.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "../3rd-party/uthash/uthash.h"

// -------------------------------------------------------------------------
// 宏定义

#define STRINGIFY(x) #x
#define MAX_TIMESTAMP_LEN 30
#define MAX_SUBMODULE_LEN 20
#define MAX_FILE_PATH_LEN 256
#define MAX_LOG_FILE_SIZE LONG_MAX
#ifdef __PROBE__
#define LOG_CONFIG_PATH "/etc/dlpos/conf/logconf.json"
#else
#define LOG_CONFIG_PATH "./conf/logconf.json"
#endif /* __PROBE__ */

// -------------------------------------------------------------------------
// 日志等级
typedef enum {
    DEBUG,
    INFO,
    WARNING,
    ERROR,

    MAX_LEVEL_NUM
} LogLevel;

// 日志等级对应的字符串，保持与LogLevel枚举一致
const char *g_dlplog_level_str_arr[] = {
    STRINGIFY(DEBUG),
    STRINGIFY(INFO),
    STRINGIFY(WARNING),
    STRINGIFY(ERROR)
};

// 通过字符串获得LogLevel枚举，保持与LogLevel枚举一致
LogLevel string2LogLevel(const char* str)
{
    if (strcmp(str, "DEBUG") == 0) {
        return DEBUG;
    } else if (strcmp(str, "INFO") == 0) {
        return INFO;
    } else if (strcmp(str, "WARNING") == 0) {
        return WARNING;
    } else if (strcmp(str, "ERROR") == 0) {
        return ERROR;
    } else {
        return MAX_LEVEL_NUM;
    }
}

// -------------------------------------------------------------------------

typedef struct {
    const char *submodule_name; // 子模块名称
    const char *logging_enable; // 是否启用日志
    const char *log_directory; // 日志目录
    const char *log_min_messages; // 日志输出的最小等级
    int log_rotation_day; // 每隔多少天进行切分
    int log_rotation_size_mb; // 文件大小达到多少进行切分，单位是MB

    long log_rotation_size_byte; // 单位是B
    const char *old_file_name; // 旧的文件名，含路径
    const char *cur_file_name; // 当前文件名，含路径
    long cur_file_size_byte; // 当前文件大小，单位是B
    FILE *file;
    UT_hash_handle hh;
} LogFile;

// -------------------------------------------------------------------------
// 获得当前时间
void get_timestamp(char *timestamp)
{
    time_t t = time(NULL);
    struct tm *currentTime = localtime(&t);
    strftime(timestamp, MAX_TIMESTAMP_LEN, "%Y-%m-%d_%H%M%S", currentTime);
}

// 获得调用栈信息
void print_call_stack(FILE *file) 
{
    void* call_stack[50];
    int stack_depth = backtrace(call_stack, 50);
    char** stack_symbols = backtrace_symbols(call_stack, stack_depth);

    if (stack_symbols == NULL) {
        fprintf(file, "无法获取调用栈信息\n");
        return;
    }

    for (int i = 0; i < stack_depth; i++) {
        fprintf(file, "%s", stack_symbols[i]);
        if (i != stack_depth - 1) {
            fprintf(file, "\n");
        }
    }

    free(stack_symbols);
}
// -------------------------------------------------------------------------

bool directory_exists(const char* path)
{
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
        return true;
    }
    return false;
}

void clear_directory(const char* path)
{
    // 删除目录下的所有文件和子目录
    char command[256];
    snprintf(command, sizeof(command), "rm -rf %s/*", path);
    system(command);
}

int create_directory(const char* path)
{
    mode_t mode = 0777; // 目录权限模式

    // 尝试创建目录
    int result = mkdir(path, mode);
    if (result == 0) {
        return 0;
    } else {
        // 目录已存在或创建失败
        struct stat st;
        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
            // 目录已存在
            return 0;
        } else {
            // 创建目录失败
            perror("Error: Failed to create directory\n");
            return -1;
        }
    }
}

int create_directories(const char* path)
{
    char tmp[256];
    char* p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = '\0';
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (create_directory(tmp) != 0)
                return -1;
            *p = '/';
        }
    }
    if (create_directory(tmp) != 0)
        return -1;

    return 0;
}

// -------------------------------------------------------------------------

void add_log_file(const char *submoduleName)
{
    // TODO
}

void update_log_file(LogFile *logFile)
{
    if (logFile->file == NULL) { // 第一次打开日志文件
        logFile->file = fopen(logFile->cur_file_name, "a");
        if (logFile->file == NULL) {
            printf("Error: cannot open log file %s\n", logFile->cur_file_name);
            return;
        }
    } else if (logFile->cur_file_size_byte >= logFile->log_rotation_size_byte) {
        // 关闭旧日志文件
        fclose(logFile->file);
        logFile->file = NULL;
        logFile->cur_file_size_byte = 0;
        if (rename(logFile->cur_file_name, logFile->old_file_name) != 0) {
            printf("Error: Failed to rename the file %s.\n", logFile->cur_file_name);
            return;
        }

        // 生成新的日志文件名
        char *log_path = strdup(logFile->log_directory);
        char file_name[MAX_FILE_PATH_LEN] = {0};
        char timestamp[MAX_TIMESTAMP_LEN] = {0};
        get_timestamp(timestamp);
        snprintf(file_name, sizeof(file_name), "%s/%s-%s.log", log_path, logFile->submodule_name, timestamp);
        logFile->old_file_name = strdup(file_name);

        // 打开新日志文件
        logFile->file = fopen(logFile->cur_file_name, "a");
        if (logFile->file == NULL) {
            printf("Error: cannot open log file %s\n", logFile->cur_file_name);
            return;
        }
    }

    // 更新日志文件大小
    fseek(logFile->file, 0, SEEK_END);
    logFile->cur_file_size_byte = ftell(logFile->file);
}

// -------------------------------------------------------------------------

#endif /* __COMMON_H__ */
