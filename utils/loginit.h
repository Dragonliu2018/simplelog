/*
 * @Author: 刘振龙 dragonliu@buaa.edu.cn
 * @Date: 2023-06-08 18:01:53
 * @LastEditors: 刘振龙 dragonliu@buaa.edu.cn
 * @LastEditTime: 2023-06-15 20:28:22
 * @FilePath: /dlplog/utils/loginit.h
 * @Description: init functions of dlplog
 */

#ifndef __LOGINIT_H__
#define __LOGINIT_H__

#include <stdlib.h>

#include "common.h"

void refactor_log_config(LogConfig *g_dlplog_config)
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

void init_log_file(LogConfig *config, LogFile **log_file_arr)
{
    for (SubmoduleName name = GLOBAL + 1; name < MAX_SUBMODULE_NUM; name++) {
        log_file_arr[name] = (LogFile *)malloc(sizeof(LogFile));
        if (log_file_arr[name] == NULL) {
            printf("Error: log_file_arr[%d]'s memory allocation failed!\n", name);
            return;
        }
        memset((void *)log_file_arr[name], 0, sizeof(log_file_arr[name]));

        OptionDetail *detail = config->option_detail_arr[name];
        if (detail == NULL) {
            printf("Error: detail is NULL!\n");
            return;
        }

        log_file_arr[name]->log_path = strdup(strdup(detail->log_directory));

        const char *name_str = g_dlplog_submodule_name_str_arr[name];
        char *file_name = malloc(strlen(log_file_arr[name]->log_path) + strlen(name_str) + MAX_TIMESTAMP_LEN);
        if (file_name == NULL) {
            printf("Error: file_name's memory allocation failed!\n");
            return;
        }
        memset(file_name, 0, strlen(file_name));
        char *timestamp = (char *)malloc(MAX_TIMESTAMP_LEN);
        if (timestamp == NULL) {
            printf("Error: timestamp's memory allocation failed!\n");
            return;
        }
        memset(timestamp, 0, strlen(timestamp));
        get_timestamp(timestamp);
        strcat(file_name, log_file_arr[name]->log_path);
        strcat(file_name, "/");
        strcat(file_name, name_str);
        strcat(file_name, "-");
        strcat(file_name, timestamp);
        strcat(file_name, ".log");
        log_file_arr[name]->old_file_name = strdup(file_name);

        memset(file_name, 0, strlen(file_name));
        strcat(file_name, log_file_arr[name]->log_path);
        strcat(file_name, "/");
        strcat(file_name, name_str);
        strcat(file_name, ".log");
        log_file_arr[name]->cur_file_name = strdup(file_name);

        if (detail->log_rotation_size == 0) {
            log_file_arr[name]->log_rotation_size_byte = MAX_LOG_FILE_SIZE;
        } else {
            log_file_arr[name]->log_rotation_size_byte = detail->log_rotation_size * 1024 * 1024;
        }
        log_file_arr[name]->file_size = 0;
        log_file_arr[name]->file = NULL;
        log_file_arr[name]->submodule_name = strdup(g_dlplog_submodule_name_str_arr[name]);
    }
}

void init_log_dir(LogFile **log_file_arr)
{
    for (SubmoduleName name = GLOBAL + 1; name < MAX_SUBMODULE_NUM; name++) {
        const char *log_path = log_file_arr[name]->log_path;

        if (directory_exists(log_path)) {
            // 目录存在，清空目录
            clear_directory(log_path);
        } else {
            // 目录不存在，创建目录
            create_directories(log_path);
        }
    }
}

#endif /* __LOGINIT_H__ */
