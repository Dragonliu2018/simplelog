#pragma once

#include "common.h"

#include <assert.h>

void init_log_dir(LogConfig *config)
{
    for (SubmoduleName name = Start_Submodule + 1; name < End_Submodule; name++) {
        const char *name_str = get_submodule_name_str(name);
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
        char *log_path = malloc(sizeof(log_dir) + sizeof("/log/") + sizeof(name_str) + 50);
        strcat(log_path, log_dir);
        strcat(log_path, "/log/");
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
