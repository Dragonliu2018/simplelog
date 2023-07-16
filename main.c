/*
 * @Author: 刘振龙 dragonliu@buaa.edu.cn
 * @Date: 2023-06-08 18:01:53
 * @LastEditors: 刘振龙 dragonliu@buaa.edu.cn
 * @LastEditTime: 2023-07-15 22:09:08
 * @FilePath: /dlplog/main.c
 * @Description: test file of dlplog
 */

#include "dlplog.h"

#define LOOP false

int main()
{
    // 日志接口测试
    int v1 = 100;
    const char *v2 = "string demo";
    float v3 = 3.1415;

    do {
        LOG_DEBUG("SAMPLING_NODE", "这是一条调试日志->%d", v1);
        LOG_INFO("ANALYSIS_NODE", "这是一条信息日志->%s", v2);
        LOG_WARNIGN("MANAGEMENT_NODE", "这是一条警告日志->%f", v3);
        LOG_ERROR("MANAGEMENT_NODE", "这是一条错误日志");
        LOG_ERROR("PROBE", "这是一条错误日志");
    } while(LOOP);

    return 0;
}
