/*
 * @Author: 刘振龙 dragonliu@buaa.edu.cn
 * @Date: 2023-06-08 18:01:53
 * @LastEditors: 刘振龙 dragonliu@buaa.edu.cn
 * @LastEditTime: 2023-06-13 10:57:55
 * @FilePath: /dlplog/utils/loginit.h
 * @Description: init functions of dlplog
 */

#ifndef __LOGINIT_H__
#define __LOGINIT_H__

#include <stdlib.h>

#include "common.h"

void refactor_config(LogConfig *g_dlplog_config)
{
    if (g_dlplog_config == NULL) {
        printf("Error: g_dlplog_config is NULL!\n");
        return;
    } else if (g_dlplog_config->log_option_arr[GLOBAL] == NULL) {
        printf("Error: g_dlplog_config->log_option_arr[GLOBAL] is NULL!\n");
        return;
    } else if (g_dlplog_config->option_detail_arr[GLOBAL] == NULL) {
        printf("Error: g_dlplog_config->option_detail_arr[GLOBAL] is NULL!\n");
        return;
    }

    for (SubmoduleName name = GLOBAL + 1; name < MAX_SUBMODULE_NUM; name++) {
        // hard code: 字符串浅拷贝，共用一块内存
        if (g_dlplog_config->log_option_arr[name] == NULL)
            g_dlplog_config->log_option_arr[name] = g_dlplog_config->log_option_arr[GLOBAL];

        if (g_dlplog_config->option_detail_arr[name] == NULL) {
            g_dlplog_config->option_detail_arr[name] = g_dlplog_config->option_detail_arr[GLOBAL];
        }
    }
}

void init_log_dir(LogConfig *config)
{
    for (SubmoduleName name = GLOBAL + 1; name < MAX_SUBMODULE_NUM; name++) {
        OptionDetail *detail = config->option_detail_arr[name];
        if (detail == NULL) {
            printf("Error: detail is NULL!\n");
            return;
        }

        const char *name_str = g_dlplog_submodule_name_str_arr[name];
        const char *log_dir = strdup(detail->log_directory);
        char *log_path = (char *)malloc(sizeof(log_dir) + sizeof(name_str) + 50);
        if (log_path == NULL) {
            printf("Error: log_path's memory allocation failed!\n");
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

void init_file_ptr_hash(LogConfig *config, FILE **g_dlplog_log_file_ptr_arr)
{
    for (SubmoduleName name = GLOBAL + 1; name < MAX_SUBMODULE_NUM; name++) {
        OptionDetail *detail = config->option_detail_arr[name];
        if (detail == NULL) {
            printf("Error: detail is NULL!\n");
            return;
        }

        const char *name_str = g_dlplog_submodule_name_str_arr[name];
        const char *log_dir = strdup(detail->log_directory);

        char *log_file = malloc(sizeof(log_dir) + sizeof(name_str) + 100);
        if (log_file == NULL) {
            printf("Error: log_file's memory allocation failed!\n");
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

        g_dlplog_log_file_ptr_arr[name] = fopen(log_file, "a");
    }
}

#endif /* __LOGINIT_H__ */
