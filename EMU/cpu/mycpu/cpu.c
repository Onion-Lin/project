#include "cpu.h"


// 全局变量初始化
uint32_t GPR[16];     // 通用寄存器组，GPR[0]固定为0
uint32_t PC;            
extern uint8_t* rom;        // ROM指针
extern uint8_t* ram;        // RAM指针


int main(int argc, char* argv[]) { 
    check(argc == 2, "Usage: %s <rom_file>", argv[0]);
    //初始化内存
    rom = rom_init(argv[1]);
    check(rom != NULL, "ROM initialization failed");
    ram = ram_init();
    check(ram != NULL, "RAM initialization failed");

    // 分配指令结构体
    ins* instruc = calloc(1, sizeof(ins));
    check_mem(instruc);

    while (1) {
        // 取指
        check(PC + 3 < 1024*2048, "PC out of ROM range: 0x%08X", PC);
        uint32_t instruction = rom_fetch(PC);

        // 译码
        instruc = CU_ID(instruction, instruc);
        check(instruc != NULL, "Instruction decode failed");

        // 执行
        execute(instruc);

        // 更新PC
        int if_jump = (instruc->func == 0b1100011);  
        pc_add(if_jump, instruc->addr);
    }

    // 释放资源
    ram_free();
    rom_free();
    free(instruc);
    return gpr_r(10); 

    error:
        if (ram) ram_free();
        if (rom) rom_free();
        if (instruc) free(instruc);
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