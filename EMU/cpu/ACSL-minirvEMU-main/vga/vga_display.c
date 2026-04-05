#include <vga_display.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;
static uint32_t pixel_buffer[512 * 512];
static bool display_initialized = false;
static bool should_quit = false;
static int vga_write_count = 0;

void vga_display_init(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return;
    }

    window = SDL_CreateWindow("MinirvEMU VGA Display",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              512, 512,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                512, 512);
    if (!texture) {
        fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // 初始化像素缓冲区为白色
    memset(pixel_buffer, 0xFF, sizeof(pixel_buffer));
    display_initialized = true;
    printf("VGA display initialized (512x512 window, 512x512 image)\n");
}

void vga_display_update(uint8_t* mem, uint32_t vga_offset, uint32_t size) {
    if (!display_initialized) return;

    // 计算在 pixel_buffer 中的偏移
    uint32_t buf_offset = vga_offset - VGA_BASE;
    uint32_t pixel_start = buf_offset / 4;
    uint32_t pixel_end = (buf_offset + size + 3) / 4;
    uint32_t max_pixels = 512 * 512;

    if (pixel_start >= max_pixels) return;
    if (pixel_end > max_pixels) pixel_end = max_pixels;

    // 从模拟器内存复制像素数据
    for (uint32_t i = pixel_start; i < pixel_end; i++) {
        uint32_t mem_addr = VGA_BASE + i * 4;
        uint32_t val = 0;
        for (int j = 0; j < 4; j++) {
            if (mem_addr + j < VGA_BASE + VGA_SIZE) {
                val |= (uint32_t)mem[mem_addr + j] << (j * 8);
            }
        }
        // 转换为 ARGB 格式 (原始格式是 RGB)
        pixel_buffer[i] = 0xFF000000 | val;
    }

    vga_write_count++;
    
    // 每 1000 次写入刷新一次显示
    if (vga_write_count % 1000 == 0) {
        vga_display_refresh();
    }
}

void vga_display_refresh(void) {
    if (!display_initialized) return;

    SDL_UpdateTexture(texture, NULL, pixel_buffer, 512 * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // 处理事件
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            should_quit = true;
        }
    }
}

void vga_display_wait(void) {
    if (!display_initialized) return;

    printf("VGA display window opened. Total VGA writes: %d\n", vga_write_count);
    printf("Close the window or press ESC to exit.\n");
    fflush(stdout);

    // 最终刷新
    vga_display_refresh();

    // 保持窗口打开直到用户关闭
    while (!should_quit) {
        SDL_Event event;
        // 使用 SDL_PollEvent 非阻塞方式
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    should_quit = true;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        should_quit = true;
                    }
                    break;
                default:
                    break;
            }
            if (should_quit) break;
        }
        SDL_Delay(100);  // 100ms 延迟，降低 CPU 使用率
    }
}

void vga_display_cleanup(void) {
    if (!display_initialized) return;

    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();

    display_initialized = false;
}
