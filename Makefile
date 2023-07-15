# 定义编译器和编译选项
CC = gcc
CFLAGS = -Wall -Wextra

# 定义目标文件
TARGET = main

# 定义源文件和对象文件
SRCS = main.c
OBJS = $(SRCS:.c=.o)

# 默认目标
all: $(TARGET)

# 生成可执行文件
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# 生成目标文件
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# 运行主程序
run:all
	./main

# 清理编译生成的文件
clean:
	rm -f $(OBJS) $(TARGET)

# 删除日志文件
clean-log:
	rm -rf ./log

# 删除全部生成的文件
clean-all:clean clean-log

.PHONY: run clean clean-log clean-all
