#ifndef __VGA_H__
#define __VGA_H__

#include <am.h>
#include <klib-macros.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 256
#define step 100
#define wtime 1000

static inline uint32_t pixel(uint8_t r, uint8_t g, uint8_t b)
{
  return (r << 16) | (g << 8) | b;
}

static inline uint8_t R(uint32_t p) { return p >> 16; }
static inline uint8_t G(uint32_t p) { return p >> 8; }
static inline uint8_t B(uint32_t p) { return p; }

//static uint32_t canvas[N][N];
// static int used[N][N];

extern uint32_t color_buf[1024000*4];       // VGA颜色缓冲区

void redraw(void);      // 将渲染好的二维图像转入一维的显存中
uint32_t update(int32_t next, int32_t state, double i);   // 更新新的一帧

#endif