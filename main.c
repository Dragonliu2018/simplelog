#include "dlplog.h"


int main()
{
    // 日志接口测试
    int v1 = 100;
    const char *v2 = "string demo";
    float v3 = 3.1415;

    LOG_DEBUG(Sampling_Node, "这是一条调试日志->%d", v1);
    LOG_INFO(Analysis_Node, "这是一条信息日志->%s", v2);
    LOG_WARNIGN(Management_Node, "这是一条警告日志->%f", v3);
    LOG_ERROR(Management_Node, "这是一条错误日志");

    return 0;
}
