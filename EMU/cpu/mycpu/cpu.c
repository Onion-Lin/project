#include "cpu.h"
#include <unistd.h>

// 函数前置声明
void clear_instruc(ins* instruc);

// 全局变量初始化
uint32_t GPR[16];     // 通用寄存器组，GPR[0]固定为0
uint32_t PC;
extern uint8_t* rom;        // ROM指针
extern uint8_t* ram;        // RAM指针
char* binfile = "./vga.bin";
uint32_t color_buf[1024000*4] = {0};         // VGA颜色缓冲区

int main() {
    assert(binfile != NULL);
    ioe_init(); // initialization for GUI

    // 初始化内存
    rom = rom_init(binfile);
    check(rom != NULL, "ROM initialization failed");
    ram = ram_init(binfile);
    check(ram != NULL, "RAM initialization failed");

    // 分配指令结构体
    ins* instruc = calloc(1, sizeof(ins));
    check_mem(instruc);

    // log prepare
    FILE* log_file = fopen("execution.log", "w");
    assert(log_file != NULL);
    check(log_file != NULL, "Failed to open log file: %s", clean_errno());

    while (1) {
        clear_instruc(instruc);

        // 检测 halt 循环
        if (PC == 0xdb0) {
            redraw();
            printf("\033[043;37m[SUCCESS]\033[0m HALT detected at PC=0x%08X\n", PC);
            fflush(stdout);

            // 释放资源
            ram_free();
            rom_free();
            free(instruc);
            fclose(log_file);
            log_file = NULL;
            sleep(100);
            break;
        }

        // 取指
        check(PC + 3 < 1024*2048, "PC out of ROM range: 0x%08X", PC);
        uint32_t instruction = rom_fetch(PC);

        // log info
        fprintf(log_file, "PC: 0x%08x, ins: 0x%08x\n", PC, instruction);

        // 译码
        instruc = CU_ID(instruction, instruc);
        check(instruc != NULL, "Instruction decode failed");
        fprintf(log_file, "\tfunc: 0x%02X, rd: %u, rs1: %u, rs2: %u, imm12: 0x%03X, imm20: 0x%05X\n",
                instruc->func, instruc->rd, instruc->rs1, instruc->rs2, instruc->imm12, instruc->imm20);
        fprintf(log_file, "\tused GPR: rd = 0x%03X, rs1=0x%03X, rs2=0x%03X\n",
                GPR[instruc->rd], GPR[instruc->rs1], GPR[instruc->rs2]);

        // 执行
        execute(instruc);
        fprintf(log_file, "\tresult: GPR%d - 0x%08X , adress: 0x%08X\n",
                instruc->rd, gpr_r(instruc->rd), instruc->addr);

        // 更新PC
        pc_add((instruc->func == 0x67), instruc->addr);
    }

    // 释放资源
    ram_free();
    rom_free();
    free(instruc);
    if (log_file) fclose(log_file);
    return gpr_r(10);

error:
    if (ram) ram_free();
    if (rom) rom_free();
    if (instruc) free(instruc);
    if (log_file) fclose(log_file);
    return 1;
}

// GPR读取
uint32_t gpr_r(uint32_t addr) {
    check(addr < 16, "GPR address out of range: %u", addr);
    return GPR[addr];

error:
    return 0;  
}

// GPR写入,自动检查并禁止写入GPR[0]
void gpr_w(uint32_t addr, uint32_t data) {
    // 1. 检查地址范围（0~15）
    if (addr >= 16) {
        log_err("GPR address out of range: %u", addr);
        debug("GPR write failed: addr=%u, data=0x%08X", addr, data);
        return;
    }

    // 2. GPR[0]硬链接为0，禁止写入
    if (addr != 0) {
        GPR[addr] = data; // 合法写操作：直接写入
    }
}

// PC更新
void pc_add(uint32_t if_jump, uint32_t jump) {
    if (if_jump) {
        PC = jump;
    } else {
        PC += 4;
    }
}

void clear_instruc (ins* instruc) {
    memset(instruc, 0, sizeof(ins));
}