/*
 * @Author: 刘振龙 dragonliu@buaa.edu.cn
 * @Date: 2023-06-08 18:01:53
 * @LastEditors: 刘振龙 dragonliu@buaa.edu.cn
 * @LastEditTime: 2023-07-24 12:32:58
 * @FilePath: /dlplog/utils/logfilehandle.h
 * @Description: parse config file
 */

#ifndef __LOGFILEHANDLE_H__
#define __LOGFILEHANDLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#ifndef cJSON__h
#include "../3rd-party/cJSON/cJSON.c"
#else
#define cJSON_IsString(a) (a->type == cJSON_String)
#define cJSON_IsObject(a) (a->type == cJSON_Object)
#define cJSON_IsNumber(a) (a->type == cJSON_Number)
#define cJSON_IsArray(a) (a->type == cJSON_Array)
#endif /* cJSON__h */

/* 初始化目录 */
void init_log_dir(const char *log_path)
{
    if (!directory_exists(log_path)) {
        // 目录不存在，创建目录
        create_directories(log_path);
    }
}

/* 解析json并初始化日志文件 */
static inline void parse_json(const char* json, LogFile **logFileHash)
{
    cJSON* root = cJSON_Parse(json);
    if (root == NULL) {
        printf("Error: Failed to parse JSON.\n");
        return;
    }

    // 获取 log_config 数组
    cJSON* log_config = cJSON_GetObjectItem(root, "log_config");
    if (log_config == NULL || !cJSON_IsArray(log_config)) {
        printf("Error: Invalid log_config.\n");
        cJSON_Delete(root);
        return;
    }

    // 遍历 log_config 数组
    int log_config_size = cJSON_GetArraySize(log_config);
    for (int i = 0; i < log_config_size; i++) {
        cJSON* item = cJSON_GetArrayItem(log_config, i);
        if (item != NULL && cJSON_IsObject(item)) {
            LogFile *lf = (LogFile *)malloc(sizeof(LogFile));
            if (lf == NULL) {
                printf("Error: lf's memory allocation failed!\n");
                return;
            }
            memset((void *)lf, 0, sizeof(lf));
            initQueue(&lf->file_name_queue);

            cJSON* submodule_name = cJSON_GetObjectItem(item, "submodule_name");
            if (submodule_name != NULL && cJSON_IsString(submodule_name)) {
                lf->submodule_name = strdup(submodule_name->valuestring);
            }

            cJSON* logging_enable = cJSON_GetObjectItem(item, "logging_enable");
            if (logging_enable != NULL && cJSON_IsString(logging_enable)) {
                lf->logging_enable = strdup(logging_enable->valuestring);
            }

            cJSON* log_directory = cJSON_GetObjectItem(item, "log_directory");
            if (log_directory != NULL && cJSON_IsString(log_directory)) {
                lf->log_directory = strdup(log_directory->valuestring);
            }

            cJSON* log_min_messages = cJSON_GetObjectItem(item, "log_min_messages");
            if (log_min_messages != NULL && cJSON_IsString(log_min_messages)) {
                lf->log_min_messages = strdup(log_min_messages->valuestring);
            }

            cJSON* log_rotation_num = cJSON_GetObjectItem(item, "log_rotation_num");
            if (log_rotation_num != NULL && cJSON_IsNumber(log_rotation_num)) {
                lf->log_rotation_num = log_rotation_num->valueint;
            }

            cJSON* log_rotation_day = cJSON_GetObjectItem(item, "log_rotation_day");
            if (log_rotation_day != NULL && cJSON_IsNumber(log_rotation_day)) {
                lf->log_rotation_day = log_rotation_day->valueint;
            }

            cJSON* log_rotation_size_mb = cJSON_GetObjectItem(item, "log_rotation_size_mb");
            if (log_rotation_size_mb != NULL && cJSON_IsNumber(log_rotation_size_mb)) {
                lf->log_rotation_size_mb = log_rotation_size_mb->valueint;
            }

            char file_name[MAX_FILE_PATH_LEN] = {0};
            char timestamp[MAX_TIMESTAMP_LEN] = {0};
            get_timestamp(timestamp);

            snprintf(file_name, sizeof(file_name), "%s/%s-%s.log", lf->log_directory, lf->submodule_name, timestamp);
            lf->old_file_name = strdup(file_name);

            snprintf(file_name, sizeof(file_name), "%s/%s.log", lf->log_directory, lf->submodule_name);
            lf->cur_file_name = strdup(file_name);

            if (lf->log_rotation_num == 0)
                lf->log_rotation_num = MAX_LOG_ROTATION_NUM;

            if (lf->log_rotation_size_mb == 0)
                lf->log_rotation_size_byte = MAX_LOG_FILE_SIZE;
            else
                lf->log_rotation_size_byte = lf->log_rotation_size_mb * 1024 * 1024;

            if (lf->log_rotation_day == 0)
                lf->log_rotation_second = MAX_LOG_ROTATION_TIME;
            else
                lf->log_rotation_second = lf->log_rotation_day * 24 * 60 * 60;

            lf->cur_file_size_byte = 0;
            lf->file = NULL;

            // 将对应submodule插入哈希表
            // TODO: 重复key插入问题
            HASH_ADD_STR(*logFileHash, submodule_name, lf);

            // 初始化目录
            init_log_dir(lf->log_directory);
        }
    }

    cJSON_Delete(root);
}

static inline void parse_json_file(const char *path, LogFile **logFileHash)
{
    // 打开文件
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("Error: Failed to open file.\n");
        return;
    }

    // 读取文件内容
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *fileContent = (char *)malloc(fileSize + 1);
    if (fileContent == NULL) {
        printf("Error: fileContent's memory allocation failed!\n");
        return;
    }
    memset(fileContent, 0, strlen(fileContent));
    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = '\0';

    // 解析 JSON
    parse_json(fileContent, logFileHash);

    // 关闭文件
    fclose(file);
    free(fileContent);
}

/* 配置文件中若没有GLOBAL，则程序自动生成 */
void handle_global_log_file(LogFile **logFileHash)
{
    LogFile *lf;
    const char *submodule_name = "GLOBAL";
    HASH_FIND_STR(*logFileHash, submodule_name, lf);
    // 程序自动生成GLOBAL
    if (lf == NULL) {
        lf = (LogFile *)malloc(sizeof(LogFile));
        if (lf == NULL) {
            printf("Error: lf's memory allocation failed!\n");
            return;
        }
        memset((void *)lf, 0, sizeof(lf));
        initQueue(&lf->file_name_queue);

        lf->submodule_name = strdup(submodule_name);
        lf->logging_enable = strdup("on");
        lf->log_directory = strdup("./log");
        lf->log_min_messages = strdup("INFO");
        lf->log_rotation_num = 0;
        lf->log_rotation_day = 1;
        lf->log_rotation_size_mb = 100;
        lf->file = NULL;

        // 将对应submodule插入哈希表
        // TODO: 重复key插入问题
        HASH_ADD_STR(*logFileHash, submodule_name, lf);
    }
}

void add_log_file(LogFile **logFileHash, const char *submoduleName)
{
    LogFile *glf;
    HASH_FIND_STR(*logFileHash, "GLOBAL", glf);
    // 程序自动生成GLOBAL
    if (glf != NULL) {
        LogFile *slf = (LogFile *)malloc(sizeof(LogFile));
        if (slf == NULL) {
            printf("Error: slf's memory allocation failed!\n");
            return;
        }
        memset((void *)slf, 0, sizeof(slf));
        initQueue(&slf->file_name_queue);

        slf->submodule_name = strdup(submoduleName);
        slf->logging_enable = strdup(glf->logging_enable);
        slf->log_directory = strdup(glf->log_directory);
        slf->log_min_messages = strdup(glf->log_min_messages);
        slf->log_rotation_day = glf->log_rotation_day;
        slf->log_rotation_size_mb = glf->log_rotation_size_mb;

        char file_name[MAX_FILE_PATH_LEN] = {0};
        char timestamp[MAX_TIMESTAMP_LEN] = {0};
        get_timestamp(timestamp);

        snprintf(file_name, sizeof(file_name), "%s/%s-%s.log", slf->log_directory, slf->submodule_name, timestamp);
        slf->old_file_name = strdup(file_name);

        snprintf(file_name, sizeof(file_name), "%s/%s.log", slf->log_directory, slf->submodule_name);
        slf->cur_file_name = strdup(file_name);

        if (slf->log_rotation_num == 0)
            slf->log_rotation_num = MAX_LOG_ROTATION_NUM;

        if (slf->log_rotation_size_mb == 0)
            slf->log_rotation_size_byte = MAX_LOG_FILE_SIZE;
        else
            slf->log_rotation_size_byte = slf->log_rotation_size_mb * 1024 * 1024;

        if (slf->log_rotation_day == 0)
            slf->log_rotation_second = MAX_LOG_ROTATION_TIME;
        else
            slf->log_rotation_second = slf->log_rotation_day * 24 * 60 * 60;

        slf->cur_file_size_byte = 0;
        slf->file = NULL;

        // 将对应submodule插入哈希表
        // TODO: 重复key插入问题
        HASH_ADD_STR(*logFileHash, submodule_name, slf);
    } else {
        printf("Error: GLOBAL log file is missing!\n");
    }
}

void update_log_file(LogFile *logFile)
{
    if (logFile->file == NULL) { // 第一次打开日志文件
        logFile->file = fopen(logFile->cur_file_name, "a");
        logFile->start_time = time(NULL);
        if (logFile->file == NULL) {
            printf("Error: cannot open log file %s\n", logFile->cur_file_name);
            return;
        }
        // 更新日志文件创建时间
        logFile->start_time = time(NULL);
    } else if ((difftime(time(NULL), logFile->start_time) > logFile->log_rotation_second) ||
               (logFile->cur_file_size_byte >= logFile->log_rotation_size_byte)) {
        // 关闭旧日志文件
        fclose(logFile->file);
        logFile->file = NULL;
        logFile->cur_file_size_byte = 0;
        if (rename(logFile->cur_file_name, logFile->old_file_name) != 0) {
            printf("Error: Failed to rename the file %s.\n", logFile->cur_file_name);
            return;
        }

        // 旧文件名进队列并判断队列是否已满
        enqueue(&logFile->file_name_queue, logFile->old_file_name);
        if (queueLength(&logFile->file_name_queue) >= logFile->log_rotation_num) {
            // 删除文件
            const char *filename = logFile->file_name_queue.front->data;
            // printf("file: %s\n", filename);
            if (remove(filename) != 0)
                printf("%s deletion failed!\n", filename);

            dequeue(&logFile->file_name_queue);
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

        // 更新日志文件创建时间
        logFile->start_time = time(NULL);
    }

    // 更新日志文件大小
    fseek(logFile->file, 0, SEEK_END);
    logFile->cur_file_size_byte = ftell(logFile->file);
}

#endif /* __LOGFILEHANDLE_H__ */
