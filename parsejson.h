#pragma once

#include "3rd-party/cJSON/cJSON.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// 用于存储json文件信息
typedef struct {
    const char *logging_enable;
    const char *log_line_prefix;
    const char *log_directory;
    const char *log_min_messages;
    const char *log_filename;
    const char *log_truncate_on_rotation;
    const char *log_rotation_age;
    int log_rotation_size;
} OptionDetail;

typedef struct {
    const char* log_option;
    OptionDetail global;
    OptionDetail submodule;
} LogConfig;

void parse_json(const char* json, LogConfig *config) {
    cJSON* root = cJSON_Parse(json);
    if (root == NULL) {
        printf("Failed to parse JSON.\n");
        return;
    }

    cJSON* logOption = cJSON_GetObjectItem(root, "log_option");
    if (logOption != NULL) {
        config->log_option = strdup(logOption->valuestring);
        printf("%s\n%s\n", config->log_option, logOption->valuestring);
    }

    // GLOBAL
    cJSON* global = cJSON_GetObjectItem(root, "GLOBAL");
    if (global != NULL) {
        cJSON* loggingEnable = cJSON_GetObjectItem(global, "logging_enable");
        if (loggingEnable != NULL) {
            config->global.logging_enable = strdup(loggingEnable->valuestring);
        }

        cJSON* logLinePrefix = cJSON_GetObjectItem(global, "log_line_prefix");
        if (logLinePrefix != NULL) {
            config->global.log_line_prefix = strdup(logLinePrefix->valuestring);
        }

        cJSON* logDirectory = cJSON_GetObjectItem(global, "log_directory");
        if (logDirectory != NULL) {
            config->global.log_directory = strdup(logDirectory->valuestring);
        }

        cJSON* logMinMessages = cJSON_GetObjectItem(global, "log_min_messages");
        if (logMinMessages != NULL) {
            config->global.log_min_messages = strdup(logMinMessages->valuestring);
        }

        cJSON* logFilename = cJSON_GetObjectItem(global, "log_filename");
        if (logFilename != NULL) {
            config->global.log_filename = strdup(logFilename->valuestring);
        }

        cJSON* logTruncateOnRotation = cJSON_GetObjectItem(global, "log_truncate_on_rotation");
        if (logTruncateOnRotation != NULL) {
            config->global.log_truncate_on_rotation = strdup(logTruncateOnRotation->valuestring);
        }

        cJSON* logRotationAge = cJSON_GetObjectItem(global, "log_rotation_age");
        if (logRotationAge != NULL) {
            config->global.log_rotation_age = strdup(logRotationAge->valuestring);
        }

        cJSON* logRotationSize = cJSON_GetObjectItem(global, "log_rotation_size");
        if (logRotationSize != NULL) {
            config->global.log_rotation_size = logRotationSize->valueint;
        }
    }

    // SUBMOUDLE
    cJSON* submodule = cJSON_GetObjectItem(root, "SUBMOUDLE");
    if (submodule != NULL) {
        cJSON* loggingEnable = cJSON_GetObjectItem(global, "logging_enable");
        if (loggingEnable != NULL) {
            config->submodule.logging_enable = strdup(loggingEnable->valuestring);
        }

        cJSON* logLinePrefix = cJSON_GetObjectItem(global, "log_line_prefix");
        if (logLinePrefix != NULL) {
            config->submodule.log_line_prefix = strdup(logLinePrefix->valuestring);
        }

        cJSON* logDirectory = cJSON_GetObjectItem(global, "log_directory");
        if (logDirectory != NULL) {
            config->submodule.log_directory = strdup(logDirectory->valuestring);
        }

        cJSON* logMinMessages = cJSON_GetObjectItem(global, "log_min_messages");
        if (logMinMessages != NULL) {
            config->submodule.log_min_messages = strdup(logMinMessages->valuestring);
        }

        cJSON* logFilename = cJSON_GetObjectItem(global, "log_filename");
        if (logFilename != NULL) {
            config->submodule.log_filename = strdup(logFilename->valuestring);
        }

        cJSON* logTruncateOnRotation = cJSON_GetObjectItem(global, "log_truncate_on_rotation");
        if (logTruncateOnRotation != NULL) {
            config->submodule.log_truncate_on_rotation = strdup(logTruncateOnRotation->valuestring);
        }

        cJSON* logRotationAge = cJSON_GetObjectItem(global, "log_rotation_age");
        if (logRotationAge != NULL) {
            config->submodule.log_rotation_age = strdup(logRotationAge->valuestring);
        }

        cJSON* logRotationSize = cJSON_GetObjectItem(global, "log_rotation_size");
        if (logRotationSize != NULL) {
            config->submodule.log_rotation_size = logRotationSize->valueint;
        }
    }

    cJSON_Delete(root);
}

void parse_json_file(const char* path, LogConfig *config) {
    // 打开文件
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        assert("Failed to open file.\n");
    }

    // 读取文件内容
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* fileContent = (char*)malloc(fileSize + 1);
    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = '\0';

    // 解析 JSON
    parse_json(fileContent, config);
    printf("%s", config->log_option);

    // 关闭文件
    fclose(file);
    free(fileContent);
}
