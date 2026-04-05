#include <am.h>
#include <klib-macros.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FPS 30
#define N 32
#define step 100
#define wtime 1000

static inline uint32_t pixel(uint8_t r, uint8_t g, uint8_t b)
{
  return (r << 16) | (g << 8) | b;
}
static inline uint8_t R(uint32_t p) { return p >> 16; }
static inline uint8_t G(uint32_t p) { return p >> 8; }
static inline uint8_t B(uint32_t p) { return p; }

static uint32_t canvas[N][N];
// static int used[N][N];

static uint32_t color_buf[32 * 32];
uint32_t src[8] = {0x000000, 0xff0000, 0x00ff00, 0x0000ff, 0xffff00, 0xff00ff, 0x00ffff, 0xffffff};

void redraw()
{ // 将渲染好的二维图像转入一维的显存中
  int w = io_read(AM_GPU_CONFIG).width / N;
  int h = io_read(AM_GPU_CONFIG).height / N;
  int block_size = w * h;
  assert((uint32_t)block_size <= LENGTH(color_buf));

  int x, y, k;
  for (y = 0; y < N; y++)
  {
    for (x = 0; x < N; x++)
    {
      for (k = 0; k < block_size; k++)
      {
        color_buf[k] = canvas[y][x];
      }
      io_write(AM_GPU_FBDRAW, x * w, y * h, color_buf, w, h, false); // 输出像素点到缓冲区
    }
  }
  io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true); // 刷新
}

/*static uint32_t p(int tsc) {
  int b = tsc & 0xff;
  return pixel(b * 6, b * 7, b);
}*/

uint32_t update(int32_t next, int32_t state, double i)
{ // 更新新的一帧
  uint8_t r = R(state) + (R(next) - R(state)) * (i / step);
  uint8_t g = G(state) + (G(next) - G(state)) * (i / step);
  uint8_t b = B(state) + (B(next) - B(state)) * (i / step);
  for (int y = 0; y < N; y++)
  {
    for (int x = 0; x < N; x++)
    {
      canvas[y][x] = pixel(r, g, b);
    }
  }

  return pixel(r, g, b);

  /*螺旋渐变逻辑
  static int tsc = 0;     //时间戳,随机数
  static int dx[4] = {0, 1, 0, -1};
  static int dy[4] = {1, 0, -1, 0};

  tsc ++;

  for (int i = 0; i < N; i ++)
    for (int j = 0; j < N; j ++) {
      used[i][j] = 0;
    }

  int init = tsc * 1;
  canvas[0][0] = p(init); used[0][0] = 1;
  int x = 0, y = 0, d = 0;
  for (int step = 1; step < N * N; step ++) {
    for (int t = 0; t < 4; t ++) {
      int x1 = x + dx[d], y1 = y + dy[d];
      if (x1 >= 0 && x1 < N && y1 >= 0 && y1 < N && !used[x1][y1]) {
        x = x1; y = y1;
        used[x][y] = 1;
        canvas[x][y] = p(init + step / 2);
        break;
      }
      d = (d + 1) % 4;
    }
  }*/
}

void video_test()
{
  unsigned long last = 0;
  unsigned long fps_last = 0;
  int fps = 0;
  double i = 0;
  uint32_t state = 0, next = 0;

  state = src[rand() % 8];
  next = src[rand() % 8];
  while (next == state)
    next = src[rand() % 8];

  while (1)
  {
    unsigned long upt = io_read(AM_TIMER_UPTIME).us / 1000;
    if (upt - last > wtime / FPS)
    {
      state = update(next, state, i);
      if (state == next)
      {
        next = src[rand() % 8];
        i = 0;
      }
      else
      {
        i += 1;
      }
      redraw();
      last = upt;
      fps++;
    }
    if (upt - fps_last > 1000)
    {
      // display fps every 1s
      printf("%d: FPS = %d\n", upt, fps);
      fps_last = upt;
      fps = 0;
    }
  }
}

int main(int argc, char *argv[])
{
  ioe_init(); // initialization for GUI
  video_test();
  return 0;
}
