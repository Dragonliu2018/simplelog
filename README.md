# simplelog
这是一个使用C语言编写的简单日志库，只需要在项目中引入`dlplog.h`头文件，然后就可以使用了。<br>
# 功能 
✅ 打印日志到日志文件 <br>
✅ 支持自定义日志存放目录 <br>
✅ 支持自定义子模块配置 <br>
✅ 支持日志输出格式化 <br>
✅ 支持日志输出关闭与开启 <br>
✅ 支持日志等级 <br>
✅ 支持日志文件按指定大小切分 <br>
✅ 支持日志文件按指定时间切分 <br>
✅ 支持日志文件覆盖 <br>
✅ 支持并发场景 <br>

# 测试
1. 将本项目clone到本地；
2. 确保本地环境已经安装gcc和make（当然make不是必须的，这里需要是为了使用项目中的Makefile）；
3. `make run` 命令是编译运行`main.c`主程序，执行完后可以在`./log`目录查看生成的日志；
4. `make clean-all` 命令是删除生成的文件，包括日志文件等。
5. 更多信息见 `Makefile`
# 使用
1. 在自己的项目中引入头文件`#include "dlplog.h"`，注意保证其路径正确；
2. 在配置文件`conf/logconf.json`中进行配置（可以不进行配置，默认使用GLOBAL配置信息）：
   ```json
    {
        "submodule_name": "xxx",    // 子模块的名称
        "logging_enable": "on",     // 是否开启日志，on表示记录日志，off表示不记录日志
        "log_directory": "./log",   // 日志文件存放目录
        "log_min_messages": "INFO", // 只打印大于等于该等级的日志，日志等级从小到大分为：DEBUG INFO WARN ERROR
        "log_rotation_num": 4,      // 每隔多少文件进行覆盖，0表示不进行覆盖
        "log_rotation_day": 1,      // 每隔多少天进行日志文件切分, 0表示不按时间切分
        "log_rotation_size_mb": 50  // 每隔多少MB进行日志文件切分，0表示不按照日志文件切分
    }
   ```
3. 如果子模块没有在配置文件中，则首先使用配置文件中的`GLOBAL`对应配置；如果配置文件中没有`GLOBAL`，则使用C代码中的默认配置信息：
   ```json
    {
        "submodule_name": "GLOBAL",
        "logging_enable": "on",
        "log_directory": "./log",
        "log_min_messages": "INFO",
        "log_rotation_num": 0,
        "log_rotation_day": 1,
        "log_rotation_size_mb": 100
    }
   ```
4. 在需要的地方进行日志接口调用，总共有下面四个日志接口，第1个参数是子模块名称，第2个参数是日志内容，支持日志输出格式化（样例见`main.c`）：
   ```c
    LOG_DEBUG("SAMPLING_NODE", "这是一条调试日志->%d", v1);
    LOG_INFO("ANALYSIS_NODE", "这是一条信息日志->%s", v2);
    LOG_WARNIGN("MANAGEMENT_NODE", "这是一条警告日志->%f", v3);
    LOG_ERROR("MANAGEMENT_NODE", "这是一条错误日志");
   ```
5. 日志文件存放在对应日志目录,以子模块名称命名的日志文件是当前文件，以子模块名称+时间命名的日志文件是旧文件。