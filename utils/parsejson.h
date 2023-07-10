/*
 * @Author: 刘振龙 dragonliu@buaa.edu.cn
 * @Date: 2023-06-08 18:01:53
 * @LastEditors: 刘振龙 dragonliu@buaa.edu.cn
 * @LastEditTime: 2023-06-18 08:50:44
 * @FilePath: /dlplog/utils/parsejson.h
 * @Description: parse config file
 */

#ifndef __PARSEJSON_H__
#define __PARSEJSON_H__

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

static inline void parse_json(const char* json, LogConfig **config)
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

    // 遍历 log_config 数组并打印每个元素的 option_name
    int log_config_size = cJSON_GetArraySize(log_config);
    for (int i = 0; i < log_config_size; i++) {
        cJSON* item = cJSON_GetArrayItem(log_config, i);
        if (item != NULL && cJSON_IsObject(item)) {
            LogConfig *lc = (LogConfig *)malloc(sizeof(LogConfig));
            if (lc == NULL) {
                printf("Error: lc's memory allocation failed!\n");
                return;
            }
            memset((void *)lc, 0, sizeof(lc));

            cJSON* option_name = cJSON_GetObjectItem(item, "option_name");
            if (option_name != NULL && cJSON_IsString(option_name)) {
                lc->option_name = strdup(option_name->valuestring);
            }

            cJSON* logging_enable = cJSON_GetObjectItem(item, "logging_enable");
            if (logging_enable != NULL && cJSON_IsString(logging_enable)) {
                lc->logging_enable = strdup(logging_enable->valuestring);
            }

            cJSON* log_directory = cJSON_GetObjectItem(item, "log_directory");
            if (log_directory != NULL && cJSON_IsString(log_directory)) {
                lc->log_directory = strdup(log_directory->valuestring);
            }

            cJSON* log_min_messages = cJSON_GetObjectItem(item, "log_min_messages");
            if (log_min_messages != NULL && cJSON_IsString(log_min_messages)) {
                lc->log_min_messages = strdup(log_min_messages->valuestring);
            }

            cJSON* log_truncate_on_rotation = cJSON_GetObjectItem(item, "log_truncate_on_rotation");
            if (log_truncate_on_rotation != NULL && cJSON_IsString(log_truncate_on_rotation)) {
                lc->log_truncate_on_rotation = strdup(log_truncate_on_rotation->valuestring);
            }

            cJSON* log_rotation_age = cJSON_GetObjectItem(item, "log_rotation_age");
            if (log_rotation_age != NULL && cJSON_IsString(log_rotation_age)) {
                lc->log_rotation_age = strdup(log_rotation_age->valuestring);
            }

            cJSON* log_rotation_size = cJSON_GetObjectItem(item, "log_rotation_size");
            if (log_rotation_size != NULL && cJSON_IsNumber(log_rotation_size)) {
                lc->log_rotation_size = log_rotation_size->valueint;
            }

            config[i] = lc;
        }
    }

    cJSON_Delete(root);
}

static inline void parse_json_file(const char *path, LogConfig **config)
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
    parse_json(fileContent, config);

    // 关闭文件
    fclose(file);
    free(fileContent);
}

#endif /* __PARSEJSON_H__ */
