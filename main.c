/*
 * @Author: 刘振龙 dragonliu@buaa.edu.cn
 * @Date: 2023-06-08 18:01:53
 * @LastEditors: 刘振龙 dragonliu@buaa.edu.cn
 * @LastEditTime: 2023-06-11 10:41:00
 * @FilePath: /dlplog/main.c
 * @Description: test file of dlplog
 */

#include "dlplog.h"

int main()
{
    // 日志接口测试
    int v1 = 100;
    const char *v2 = "string demo";
    float v3 = 3.1415;

    LOG_DEBUG(SAMPLING_NODE, "这是一条调试日志->%d", v1); // 宏定义+枚举全大写
    LOG_INFO(ANALYSIS_NODE, "这是一条信息日志->%s", v2);
    LOG_WARNIGN(MANAGEMENT_NODE, "这是一条警告日志->%f", v3);
    LOG_ERROR(MANAGEMENT_NODE, "这是一条错误日志");

    return 0;
}
