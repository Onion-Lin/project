# 编译器选项
CFLAGS =  -O2 -g

# 目标可执行文件名
TARGET = mycpu

# 源码目录（当前目录）和输出目录
SRCDIR = .
OBJDIR = build

# 获取所有 .c 源文件
SOURCES = $(wildcard $(SRCDIR)/*.c)

# 将 .c 替换为 .o，并加上 build/ 前缀
OBJECTS = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.c=.o)))

# 依赖文件列表（与 .o 一一对应）
DEPENDS = $(OBJECTS:.o=.d)

# 默认目标
$(TARGET): $(OBJECTS)
	gcc $(CFLAGS) -o $@ $^

# 编译规则：每个 .c 生成对应的 .o 和 .d 到 build/ 目录
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	gcc $(CFLAGS) -c -o $@ $< -MMD -MF $(@:.o=.d)

# 包含依赖文件（如果存在）
-include $(DEPENDS)

# 清理规则
clean:
	rm -rf $(TARGET) $(OBJDIR) execution.log