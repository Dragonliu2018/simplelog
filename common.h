#pragma once

#include "3rd-party/uthash/uthash.h"

#include <stdio.h>
#include <time.h>
#include <execinfo.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <stdbool.h>

// -------------------------------------------------------------------------
// 用于存储json文件信息

typedef struct {
    const char *option_name; // key
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
    ERROR
} LogLevel;

// TODO: 补充子模块名称
// Note: 在起止标志之间增加子模块
// 子模块名称
typedef enum {
    Start_Submodule, // 枚举中的起始标志

    Sampling_Node, // 采集节点
    Analysis_Node, // 分析节点
    Management_Node, // 管理节点

    End_Submodule, // 枚举中的结束标志
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
