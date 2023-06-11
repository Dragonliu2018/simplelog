/*
 * @Author: 刘振龙 dragonliu@buaa.edu.cn
 * @Date: 2023-06-08 18:01:53
 * @LastEditors: 刘振龙 dragonliu@buaa.edu.cn
 * @LastEditTime: 2023-06-11 11:44:32
 * @FilePath: /dlplog/utils/loginit.h
 * @Description: init functions of dlplog
 */

#ifndef __LOGINIT_H__
#define __LOGINIT_H__

#include <assert.h>

#include "common.h"

void init_log_dir(LogConfig *config)
{
    for (SubmoduleName name = 0; name < MAX_SUBMODULE_NUM; name++) {
        const char *name_str = g_dlplog_submodule_name_str_arr[name];
        LogOption *option;
        OptionDetail *detail;
        HASH_FIND_STR(config->log_options, name_str, option);  // 根据option_name查找option
        if (option == NULL) {
            HASH_FIND_STR(config->option_details, "GLOBAL", detail);  // 根据option_name查找detail
            assert(detail != NULL);
        } else {
            HASH_FIND_STR(config->option_details, name_str, detail);  // 根据option_name查找detail
            assert(detail != NULL);
        }
        const char *log_dir = strdup(detail->log_directory);
        char *log_path = malloc(sizeof(log_dir) + sizeof(name_str) + 50);
        if (log_path == NULL) {
            printf("log_path's memory allocation failed!\n");
            return;
        }
        memset(log_path, 0, strlen(log_path));
        strcat(log_path, log_dir);
        strcat(log_path, "/");
        strcat(log_path, name_str);

        if (directory_exists(log_path)) {
            // 目录存在，清空目录
            clear_directory(log_path);
        } else {
            // 目录不存在，创建目录
            create_directories(log_path);
        }
    }
}

void init_file_ptr_hash(LogConfig *config, LogFile **logFileHash)
{
    for (SubmoduleName name = 0; name < MAX_SUBMODULE_NUM; name++) {
        const char *name_str = g_dlplog_submodule_name_str_arr[name];
        LogOption *option;
        OptionDetail *detail;
        HASH_FIND_STR(config->log_options, name_str, option);  // 根据option_name查找option option指针或指针的指针
        if (option == NULL) {
            HASH_FIND_STR(config->option_details, "GLOBAL", detail);  // 根据option_name查找detail
            assert(detail != NULL);
        } else {
            HASH_FIND_STR(config->option_details, name_str, detail);  // 根据option_name查找detail
            assert(detail != NULL);
        }
        const char *log_dir = strdup(detail->log_directory);

        char *log_file = malloc(sizeof(log_dir) + sizeof(name_str) + 100);
        if (log_file == NULL) {
            printf("log_file's memory allocation failed!\n");
            return;
        }
        memset(log_file, 0, strlen(log_file));
        char *timestamp = (char *)malloc(MAX_TIMESTAMP_LEN);
        memset(timestamp, 0, strlen(timestamp));
        get_timestamp(timestamp);
        strcat(log_file, log_dir);
        strcat(log_file, "/");
        strcat(log_file, name_str);
        strcat(log_file, "/dlp-");
        strcat(log_file, timestamp);
        strcat(log_file, ".log");

        LogFile *sub_file = (LogFile *)malloc(sizeof(LogFile));
        if (sub_file == NULL) {
            printf("sub_file's memory allocation failed!\n");
            return;
        }
        memset((void *)sub_file, 0, sizeof(sub_file));
        sub_file->option_name = strdup(name_str);
        sub_file->file = fopen(log_file, "a");
        HASH_ADD_STR(*logFileHash, option_name, sub_file);
    }
}

#endif /* __LOGINIT_H__ */
