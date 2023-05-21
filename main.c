#include "dlplog.h"

int main()
{
    printf("log starts:\n");

    // 记录日志消息
    LOG_DEBUG(Sampling_Node, "这是一条调试日志");
    LOG_INFO(Analysis_Node, "这是一条信息日志");
    LOG_WARNIGN(Management_Node, "这是一条警告日志");
    LOG_ERROR(Management_Node, "这是一条错误日志");

    return 0;
}