#include "cpu.h"

// 硬件指针
extern uint8_t* rom = NULL;
extern uint8_t* ram = NULL;

// ROM初始化
uint8_t* rom_init(char* ROMFILE) {
    FILE* fp = fopen(ROMFILE, "rb");
    check(fp != NULL, "Failed to open rom.bin: %s", clean_errno());  // 新增文件检查

    rom = calloc(ROM_SIZE, sizeof(uint8_t));  // ROM大小2MB
    check_mem(rom);

    size_t read_bytes = fread(rom, sizeof(uint8_t), ROM_SIZE, fp);
    fclose(fp);
    check(read_bytes < ROM_SIZE, "ROM read incomplete: %zu bytes", read_bytes);

    rom[ebrekaddr] = 0x73;  // 在halt地址放置ebreak指令（opcode 0x73）
    rom[ebrekaddr + 1] = 0x00;
    rom[ebrekaddr + 2] = 0x10;
    rom[ebrekaddr + 3] = 0x00;
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

/*uint32_t rom_fetch(uint32_t PC) {
    // 检查PC是否4字节对齐（RISC-V 32位指令必须对齐）
    check((PC & 0x3) == 0, "PC is not 4-byte aligned: 0x%08X", PC);
    // 小端序拼接：低地址字节 = 指令低8位
    uint32_t instruction = rom[PC] | (rom[PC+1] << 8) | (rom[PC+2] << 16) | (rom[PC+3] << 24);
    return instruction;

error:
    return 0;
}*/


void rom_free() {
    if (rom) {
        free(rom);
        rom = NULL;
    }
}

// RAM初始化
uint8_t* ram_init() {
    ram = calloc(RAM_SIZE, sizeof(uint8_t));  // RAM大小2MB
    check(ram != NULL, "RAM allocation failed");
    log_info("RAM initialized (2MB)");
    return ram;

error:
    if (ram) free(ram);
    ram = NULL;
    return NULL;
}

// RAM读取
uint8_t ram_read(uint32_t addr) {
    check(addr < RAM_SIZE, "RAM read out of range: 0x%08X", addr);
    return ram[addr];

error:
    return 0;
}

// RAM写入
void ram_write(uint32_t addr, uint8_t data) {
    check(addr < RAM_SIZE, "RAM write out of range: 0x%08X", addr);
    ram[addr] = data;

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