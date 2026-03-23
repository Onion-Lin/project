#!/bin/bash

# 检查参数数量
if [ $# -ne 2 ]; then
    echo "Usage: $0 <input.hex> <output.bin>"
    exit 1
fi

HEX_FILE="$1"
BIN_FILE="$2"

# 检查输入文件是否存在
if [ ! -f "$HEX_FILE" ]; then
    echo "Error: Input file '$HEX_FILE' not found"
    exit 1
fi

# 处理 hex 文件并转换为 bin
tail -n +2 "$HEX_FILE" | sed -e 's/.*: //' -e 's/ /\n/g' | sed -e 's/\(..\)\(..\)\(..\)\(..\)/\4 \3 \2 \1/' | xxd -r -p > "$BIN_FILE"