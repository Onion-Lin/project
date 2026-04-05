#include "vga.h"

#define N 256

void redraw() {
    // 获取屏幕尺寸
    int screen_w = io_read(AM_GPU_CONFIG).width;
    int screen_h = io_read(AM_GPU_CONFIG).height;
    
    // 分块大小
    int block_w = screen_w / N;
    int block_h = screen_h / N;
    int block_size = block_w * block_h;
    
    // 确保块大小不超过 color_buf
    if ((uint32_t)block_size > LENGTH(color_buf)) {
        block_size = LENGTH(color_buf);
    }
    
    // 分块绘制
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < N; x++) {
            // 计算当前块在 color_buf 中的起始位置
            uint32_t buf_offset = (y * N + x) * block_size;
            
            // 如果偏移量超出缓冲区，跳过
            if (buf_offset >= LENGTH(color_buf)) continue;
            
            // 绘制当前块
            io_write(AM_GPU_FBDRAW, x * block_w, y * block_h, 
                    &color_buf[buf_offset], block_w, block_h, false);
        }
    }
    
    // 刷新显示
    io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true);
}



