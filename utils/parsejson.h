/*
 * @Author: 刘振龙 dragonliu@buaa.edu.cn
 * @Date: 2023-06-08 18:01:53
 * @LastEditors: 刘振龙 dragonliu@buaa.edu.cn
 * @LastEditTime: 2023-06-11 14:42:07
 * @FilePath: /dlplog/utils/parsejson.h
 * @Description: parse config file
 */

#ifndef __PARSEJSON_H__
#define __PARSEJSON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "../3rd-party/cJSON/cJSON.c"

void parse_json(const char* json, LogConfig *config)
{
    cJSON* root = cJSON_Parse(json);
    if (root == NULL) {
        printf("Error: Failed to parse JSON.\n");
        return;
    }

    // 获取 log_option 数组
    cJSON* log_option = cJSON_GetObjectItem(root, "log_option");
    if (log_option == NULL || !cJSON_IsArray(log_option)) {
        printf("Error: Invalid log_option.\n");
        cJSON_Delete(root);
        return;
    }

    // 遍历 log_option 数组并打印每个元素
    int log_option_size = cJSON_GetArraySize(log_option);
    for (int i = 0; i < log_option_size; i++) {
        cJSON *item = cJSON_GetArrayItem(log_option, i);
        if (item != NULL && cJSON_IsString(item)) {
            SubmoduleName sname = string2SubmoduleName(item->valuestring);
            if (sname != MAX_SUBMODULE_NUM) {
                config->log_option_arr[sname] = strdup(item->valuestring);
            } else {
                printf("Error: Invalid SubmoduleName: %s\n", item->valuestring);
            }
        }
    }

    // 获取 option_details 数组
    cJSON* option_details = cJSON_GetObjectItem(root, "option_details");
    if (option_details == NULL || !cJSON_IsArray(option_details)) {
        printf("Error: Invalid option_details.\n");
        cJSON_Delete(root);
        return;
    }

    // 遍历 option_details 数组并打印每个元素的 option_name
    int option_details_size = cJSON_GetArraySize(option_details);
    for (int i = 0; i < option_details_size; i++) {
        cJSON* item = cJSON_GetArrayItem(option_details, i);
        if (item != NULL && cJSON_IsObject(item)) {
            OptionDetail *od = (OptionDetail *)malloc(sizeof(OptionDetail));
            if (od == NULL) {
                printf("Error: od's memory allocation failed!\n");
                return;
            }
            memset((void *)od, 0, sizeof(od));

            cJSON* option_name = cJSON_GetObjectItem(item, "option_name");
            if (option_name != NULL && cJSON_IsString(option_name)) {
                od->option_name = strdup(option_name->valuestring);
            }

            cJSON* logging_enable = cJSON_GetObjectItem(item, "logging_enable");
            if (logging_enable != NULL && cJSON_IsString(logging_enable)) {
                od->logging_enable = strdup(logging_enable->valuestring);
            }

            cJSON* log_directory = cJSON_GetObjectItem(item, "log_directory");
            if (log_directory != NULL && cJSON_IsString(log_directory)) {
                od->log_directory = strdup(log_directory->valuestring);
            }

            cJSON* log_min_messages = cJSON_GetObjectItem(item, "log_min_messages");
            if (log_min_messages != NULL && cJSON_IsString(log_min_messages)) {
                od->log_min_messages = strdup(log_min_messages->valuestring);
            }

            cJSON* log_filename = cJSON_GetObjectItem(item, "log_filename");
            if (log_filename != NULL && cJSON_IsString(log_filename)) {
                od->log_filename = strdup(log_filename->valuestring);
            }

            cJSON* log_truncate_on_rotation = cJSON_GetObjectItem(item, "log_truncate_on_rotation");
            if (log_truncate_on_rotation != NULL && cJSON_IsString(log_truncate_on_rotation)) {
                od->log_truncate_on_rotation = strdup(log_truncate_on_rotation->valuestring);
            }

            cJSON* log_rotation_age = cJSON_GetObjectItem(item, "log_rotation_age");
            if (log_rotation_age != NULL && cJSON_IsString(log_rotation_age)) {
                od->log_rotation_age = strdup(log_rotation_age->valuestring);
            }

            cJSON* log_rotation_size = cJSON_GetObjectItem(item, "log_rotation_size");
            if (log_rotation_size != NULL && cJSON_IsNumber(log_rotation_size)) {
                od->log_rotation_size = log_rotation_size->valueint;
            }

            SubmoduleName sname = string2SubmoduleName(od->option_name);
            if (sname != MAX_SUBMODULE_NUM) {
                config->option_detail_arr[sname] = od;
            } else {
                printf("Error: Invalid SubmoduleName: %s\n", od->option_name);
            }
        }
    }

    cJSON_Delete(root);
}

void parse_json_file(const char *path, LogConfig **config)
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
    *config = (LogConfig *)malloc(sizeof(LogConfig));
    if (*config == NULL) {
        printf("Error: *config's memory allocation failed!\n");
        return;
    }
    memset((void *)*config, 0, sizeof(*config));
    parse_json(fileContent, *config);

    // 关闭文件
    fclose(file);
    free(fileContent);
}

#endif /* __PARSEJSON_H__ */
