/*
 * @Author: 刘振龙 dragonliu@buaa.edu.cn
 * @Date: 2023-06-08 18:01:53
 * @LastEditors: 刘振龙 dragonliu@buaa.edu.cn
 * @LastEditTime: 2023-06-11 11:52:05
 * @FilePath: /dlplog/utils/common.h
 * @Description: common parts of dlplog
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <execinfo.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

#include "../3rd-party/uthash/uthash.h"

// -------------------------------------------------------------------------
// 宏定义

#define STRINGIFY(x) #x
#define MAX_TIMESTAMP_LEN 30
#define LOG_CONFIG_PATH "./conf/logconf.json"

// -------------------------------------------------------------------------
// 用于存储json文件信息

typedef struct {
    const char *option_name; // key，改成数组
    UT_hash_handle hh;  // 必须包含这个字段来使用uthash库
} LogOption;

typedef struct {
    const char *option_name; // key
    const char *logging_enable;
    const char *log_directory;
    const char *log_min_messages;
    const char *log_filename;
    const char *log_truncate_on_rotation;
    const char *log_rotation_age;
    int log_rotation_size;
    UT_hash_handle hh;  // 必须包含这个字段来使用uthash库
} OptionDetail;

typedef struct {
    LogOption *log_options;
    OptionDetail *option_details;
} LogConfig;

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

// TODO: 补充子模块名称
// Note: 在MAX_SUBMODULE_NUM之前增加子模块
// 子模块名称
typedef enum {
    SAMPLING_NODE, // 采集节点
    ANALYSIS_NODE, // 分析节点
    MANAGEMENT_NODE, // 管理节点

    MAX_SUBMODULE_NUM
} SubmoduleName;

// 子模块对应的字符串，保持与SubmoduleName枚举一致
const char *g_dlplog_submodule_name_str_arr[] = {
    STRINGIFY(SAMPLING_NODE),
    STRINGIFY(ANALYSIS_NODE),
    STRINGIFY(MANAGEMENT_NODE)
};

// 获得当前时间
void get_timestamp(char *timestamp)
{
    time_t t = time(NULL);
    struct tm *currentTime = localtime(&t);
    strftime(timestamp, MAX_TIMESTAMP_LEN, "%Y-%m-%d_%H:%M:%S", currentTime);
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
        fprintf(file, "%s\n", stack_symbols[i]);
    }

    free(stack_symbols);
}
// -------------------------------------------------------------------------

bool directory_exists(const char* path) {
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
        return true;
    }
    return false;
}

void clear_directory(const char* path) {
    // 删除目录下的所有文件和子目录
    char command[256];
    snprintf(command, sizeof(command), "rm -rf %s/*", path);
    system(command);
}

int create_directory(const char* path) {
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
            perror("Failed to create directory");
            return -1;
        }
    }
}

int create_directories(const char* path) {
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

typedef struct {
    const char *option_name; // key
    FILE *file;
    UT_hash_handle hh;  // 必须包含这个字段来使用uthash库
} LogFile;

// -------------------------------------------------------------------------

#endif /* __COMMON_H__ */
