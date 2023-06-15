/*
 * @Author: 刘振龙 dragonliu@buaa.edu.cn
 * @Date: 2023-06-08 18:01:53
 * @LastEditors: 刘振龙 dragonliu@buaa.edu.cn
 * @LastEditTime: 2023-06-15 21:26:45
 * @FilePath: /dlplog/utils/loginit.h
 * @Description: init functions of dlplog
 */

#ifndef __LOGINIT_H__
#define __LOGINIT_H__

#include <stdlib.h>

#include "common.h"

bool refactor_log_config(LogConfig *g_dlplog_config)
{
    if (g_dlplog_config == NULL) {
        printf("Error: g_dlplog_config is NULL!\n");
        return false;
    } else if (g_dlplog_config->log_option_arr[GLOBAL] == NULL) {
        printf("Error: g_dlplog_config->log_option_arr[GLOBAL] is NULL!\n");
        return false;
    } else if (g_dlplog_config->option_detail_arr[GLOBAL] == NULL) {
        printf("Error: g_dlplog_config->option_detail_arr[GLOBAL] is NULL!\n");
        return false;
    }

    for (SubmoduleName name = GLOBAL + 1; name < MAX_SUBMODULE_NUM; name++) {
        // hard code: 字符串浅拷贝，共用一块内存
        if (g_dlplog_config->log_option_arr[name] == NULL)
            g_dlplog_config->log_option_arr[name] = g_dlplog_config->log_option_arr[GLOBAL];

        if (g_dlplog_config->option_detail_arr[name] == NULL) {
            g_dlplog_config->option_detail_arr[name] = g_dlplog_config->option_detail_arr[GLOBAL];
        }
    }

    return true;
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
        char file_name[MAX_FILE_PATH_LEN] = {0};
        char timestamp[MAX_TIMESTAMP_LEN] = {0};
        get_timestamp(timestamp);

        snprintf(file_name, sizeof(file_name), "%s/%s-%s.log", log_file_arr[name]->log_path, name_str, timestamp);
        log_file_arr[name]->old_file_name = strdup(file_name);

        snprintf(file_name, sizeof(file_name), "%s/%s.log", log_file_arr[name]->log_path, name_str);
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

        if (!directory_exists(log_path)) {
            // 目录不存在，创建目录
            create_directories(log_path);
        }
    }
}

#endif /* __LOGINIT_H__ */
