#include "cpu.h"

// 硬件指针
uint8_t* rom = NULL;
uint8_t* ram = NULL;

// ROM初始化
uint8_t* rom_init(char* ROMFILE) {
    FILE* fp = fopen(ROMFILE, "rb");
    check(fp != NULL, "Failed to open rom.bin: %s", clean_errno());  // 新增文件检查

    rom = calloc(ROM_SIZE, sizeof(uint8_t));  // ROM大小2MB
    check_mem(rom);

    size_t read_bytes = fread(rom, sizeof(uint8_t), ROM_SIZE, fp);
    fclose(fp);
    check(read_bytes < ROM_SIZE, "ROM read incomplete: %zu bytes", read_bytes);

    /*rom[ebrekaddr] = 0x73;  // 在halt地址放置ebreak指令（opcode 0x73）
    rom[ebrekaddr + 1] = 0x00;
    rom[ebrekaddr + 2] = 0x10;
    rom[ebrekaddr + 3] = 0x00;*/

    log_info("ROM initialized, read %zu bytes", read_bytes);
    return rom;

error:
    if (fp) fclose(fp);
    if (rom) free(rom);
    rom = NULL;
    return NULL;
}

// ROM取值
uint32_t rom_fetch(uint32_t PC) {
    uint32_t instruction = rom[PC] | (rom[PC+1] << 8) | (rom[PC+2] << 16) | (rom[PC+3] << 24);
    return instruction;

error:
    return 0;  // 越界返回空指令
}


void rom_free() {
    if (rom) {
        free(rom);
        rom = NULL;
    }
}

// RAM初始化
uint8_t* ram_init(char* ROMFILE) {
    FILE* fp = fopen(ROMFILE, "rb");
    check(fp != NULL, "Failed to open rom.bin: %s", clean_errno()); 

    ram = calloc(RAM_SIZE, sizeof(uint8_t));  // RAM大小2MB
    log_info("RAM initialized (2MB)");
    check_mem(ram);

    size_t read_bytes = fread(ram, sizeof(uint8_t), RAM_SIZE, fp);
    fclose(fp);

    log_info("RAM initialization success , read %zu bytes", read_bytes);
    return ram;

error:
    if (fp) fclose(fp);
    if (ram) free(ram);
    ram = NULL;
    return NULL;
}

// RAM读取
uint8_t ram_read(uint32_t addr) {
    check(ram != NULL, "RAM is NULL in ram_read");
    check(addr < RAM_SIZE, "RAM read out of range: 0x%08X", addr);
    return ram[addr];

error:
    return 0;
}

// RAM 写入
void ram_write(uint32_t addr, uint8_t data) {
    check(ram != NULL, "RAM is NULL in ram_write");

    if (addr < 0x20000000) {        // mem
        check(addr < RAM_SIZE, "RAM write out of range: 0x%08X", addr);
        ram[addr] = data;
    } else {                        // vga
        check(addr < 0x20040000, "VGA write out of range: 0x%08X", addr);
        // VGA 内存映射：按字节写入，但 color_buf 是 uint32_t 数组
        // 需要将 4 个字节组合成一个 32 位像素值
        uint32_t pixel_idx = (addr - 0x20000000) / 4;
        uint32_t byte_offset = (addr - 0x20000000) % 4;
        
        // 读取当前像素值
        uint32_t current_pixel = color_buf[pixel_idx];
        
        // 修改对应的字节
        current_pixel = (current_pixel & ~(0xFF << (byte_offset * 8))) | (data << (byte_offset * 8));
        color_buf[pixel_idx] = current_pixel;
    }

error:
    return;
}

// RAM释放
void ram_free() {
    if (ram) {
        free(ram);
        ram = NULL;
        log_info("RAM freed");
    }
}