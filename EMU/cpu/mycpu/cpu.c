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
    ram = ram_init(argv[1]);
    check(ram != NULL, "RAM initialization failed");

    //printf("nononononononononono\n");
    //fflush(stdout);

    // 分配指令结构体
    ins* instruc = calloc(1, sizeof(ins));
    check_mem(instruc);


    //printf("in here\n");

    //count
    int cycle_count = 0;          
    const int MAX_CYCLES = 10000; 

    //log prepare
    FILE* log_file = fopen("execution.log", "w");
    check(log_file != NULL, "Failed to open log file: %s", clean_errno());
    log_info("Starting execution loop, max cycles: %d", MAX_CYCLES);

    while (1) {
        clear_instruc(instruc);  

        // 超过最大周期
        cycle_count++;
        if (cycle_count > MAX_CYCLES) {
            log_err("\033[41;37m[ERROR]\033[0mDead loop detected: exceed max cycles (%d)", MAX_CYCLES);
            break; 
        }

        // PC越界检查
        if (PC >= 1024*2048) { 
            log_warn("PC out of range or unaligned: 0x%08X, exit loop", PC);
            break;
        }

        if(PC == 0x1218){       //HALT,exit

            printf("\033[043;37m[SUCCESS]\033[0m HALT_Success: PC reached 0x1218 and entered halt loop\n");
            fflush(stdout);
            fprintf(log_file, "\033[043;37m[SUCCESS]\033[0m HALT_Success: PC reached 0x1218 and reached halt loop\n");
            fprintf(log_file, "\tTotal cycles: %d, a0: 0x%08X\n", cycle_count, GPR[10]);
            fprintf(log_file, "\tFinal GPR state:\n");
            for (int i = 0; i < 16; i++) {  
                fprintf(log_file, "\tGPR[%d] = 0x%08X\n", i, GPR[i]);
            }

            break;
        }

        // 取指
        check(PC + 3 < 1024*2048, "PC out of ROM range: 0x%08X", PC);
        uint32_t instruction = rom_fetch(PC);

        //log info
        fprintf(log_file, "PC: 0x%08x, ins: 0x%08x\n",PC,rom_fetch(PC));

        // 译码
        instruc = CU_ID(instruction, instruc);
        fprintf(log_file, "\tfunc: 0x%02X, rd: %u, rs1: %u, rs2: %u, imm12: 0x%03X, imm20: 0x%05X\n", instruc->func, instruc->rd, instruc->rs1, instruc->rs2, instruc->imm12, instruc->imm20);
        check(instruc != NULL, "Instruction decode failed");
        fprintf(log_file,"\tused GPR: rd = 0x%03X, rs1=0x%03X, rs2=0x%03X\n", GPR[instruc->rd], GPR[instruc->rs1], GPR[instruc->rs2]);

        // 执行
        execute(instruc);
        fprintf(log_file, "\tresult: GPR%d - 0x%08X , adress: 0x%08X\n", instruc->rd, gpr_r(instruc->rd), instruc->addr);

        // 更新PC
        fprintf(log_file, "\tfunc: 0x%02X, rd: %u, rs1: %u, rs2: %u, imm12: 0x%03X, imm20: 0x%05X\n", instruc->func, instruc->rd, instruc->rs1, instruc->rs2, instruc->imm12, instruc->imm20);
        pc_add((instruc->func == 0x67),instruc->addr);
        /*if(instruc->func != 0b1100011){ 
            pc_add(0,0);
        } */
    }

    // 释放资源
    ram_free();
    rom_free();
    free(instruc);
    fclose(log_file);
    log_info("Execution finished, total cycles: %d,a0: 0x%08X", cycle_count, GPR[10]);
    return gpr_r(10); 

    error:
        if (ram) ram_free();
        if (rom) rom_free();
        if (instruc) free(instruc);
        if (log_file) fclose(log_file);
        log_info("Execution finished, total cycles: %d,a0: 0x%08X", cycle_count, GPR[10]);
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