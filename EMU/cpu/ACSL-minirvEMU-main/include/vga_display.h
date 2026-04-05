#ifndef VGA_DISPLAY_H
#define VGA_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

// VGA 显存基地址
#define VGA_BASE 0x20000000
// VGA 显存大小 (512*512*4 字节，足够容纳大图像)
#define VGA_SIZE (512 * 512 * 4)

// 初始化 VGA 显示
void vga_display_init(void);

// 更新像素缓冲区（从模拟器内存复制）
void vga_display_update(uint8_t* mem, uint32_t vga_offset, uint32_t size);

// 刷新显示
void vga_display_refresh(void);

// 等待用户关闭窗口
void vga_display_wait(void);

// 清理资源
void vga_display_cleanup(void);

#endif
