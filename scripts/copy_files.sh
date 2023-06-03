#!/bin/bash

# 检查传入的参数数量
if [ $# -ne 1 ]; then
    echo "Usage: $0 <destination_path>"
    exit 1
fi

destination_path="$1"

# 检查目标路径是否存在
if [ ! -d "$destination_path" ]; then
    echo "Destination path does not exist or is not a directory."
    exit 1
fi

# 复制当前目录下的所有文件到目标路径
cp -rf ../3rd-party "$destination_path"
echo "3rd-party copied successfully to $destination_path"

cp -rf ../utils "$destination_path"
echo "utils copied successfully to $destination_path"

cp -rf ../conf "$destination_path"
echo "conf copied successfully to $destination_path"

cp ../dlplog.h "$destination_path"
echo "dlplog.h copied successfully to $destination_path"

cp ../main.c "$destination_path"
echo "main.c copied successfully to $destination_path"

cp ../README.md "$destination_path"
echo "README.md copied successfully to $destination_path"
