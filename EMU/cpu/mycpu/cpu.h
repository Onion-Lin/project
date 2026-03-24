#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "dbg.h"
#include "memory.h"

// 指令结构体定义
typedef struct instructions {
    uint32_t func;       // opcode
    uint32_t rd;        
    uint32_t rs1;    
    uint32_t rs2;      
    uint32_t imm12;        // 12bit立即数
    uint32_t imm20;        // 20bit立即数
    uint32_t rs;        
    uint32_t addr;    
    uint32_t funct3;     // uw or funct3
} ins;

// 全局元件声明
extern uint32_t GPR[16];     // 寄存器
extern uint32_t PC;          
extern uint8_t* rom;         // ROM
extern uint8_t* ram;         // RAM


// 函数声明
void execute(ins* instruc);
ins* CU_ID(uint32_t instruction,ins* instruc);
uint32_t rom_fetch(uint32_t PC);                
void pc_add(uint32_t if_jump, uint32_t jump);  
uint32_t gpr_r(uint32_t addr);                  
void gpr_w(uint32_t addr, uint32_t data);    

#define ebrekaddr 0x1218  // 定义ebreak指令的地址:halt函数处
//mem:0x1218
//sum:

//宏声明
// 通用符号扩展函数（替代宏，更易调试）
static inline uint32_t SEXT(uint32_t x, int len) {
    int32_t sign_bit = 1 << (len - 1); // 符号位
    // 如果符号位为1，高位补1；否则补0
    return (x & (sign_bit - 1)) | ((x & sign_bit) ? (~(sign_bit - 1)) : 0);
}
//#define SEXT(x, len) ({ struct { int32_t n : len; } __x = { .n = x }; (uint32_t)__x.n; })  //符号扩展len位x至32位
#endif