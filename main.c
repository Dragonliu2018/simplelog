/*
 * @Author: 刘振龙 dragonliu@buaa.edu.cn
 * @Date: 2023-06-08 18:01:53
 * @LastEditors: 刘振龙 dragonliu@buaa.edu.cn
 * @LastEditTime: 2023-07-24 14:04:51
 * @FilePath: /dlplog/main.c
 * @Description: test file of dlplog
 */

#include <pthread.h>
#include <unistd.h>

#include "dlplog.h"

#define LOOP false

void *log_thread(void *arg) {
    const char *info_str = (const char *)arg;
    for (int i = 0; i < 10; ++i) {
        LOG_INFO("ANALYSIS_NODE", "并发测试：信息日志->%s", info_str);
        LOG_ERROR("MANAGEMENT_NODE", "并发测试：这是一条错误日志");
    }
    return NULL;
}

int main()
{
    // 日志接口测试
    int v1 = 100;
    const char *v2 = "string demo";
    float v3 = 3.1415;

    // 并发测试
    {  
        // 创建并发测试线程
        pthread_t thread1, thread2;

        // 参数传递给线程
        const char *arg1 = "Thread 1";
        const char *arg2 = "Thread 2";

        pthread_create(&thread1, NULL, log_thread, (void *)arg1);
        pthread_create(&thread2, NULL, log_thread, (void *)arg2);

        // 等待线程执行完毕
        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
    }

    do {
        LOG_DEBUG("SAMPLING_NODE", "这是一条调试日志->%d", v1);
        LOG_INFO("ANALYSIS_NODE", "这是一条信息日志->%s", v2);
        LOG_WARNIGN("MANAGEMENT_NODE", "这是一条警告日志->%f", v3);
        LOG_ERROR("MANAGEMENT_NODE", "这是一条错误日志");
        LOG_ERROR("PROBE", "这是一条错误日志");
    } while(LOOP);

    return 0;
}
