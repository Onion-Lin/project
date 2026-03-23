#include "cpu.h"


// CU译码函数
ins* CU_ID(uint32_t instruction,ins* instruc) {
    instruc->func = instruction & 0b01111111; // 提取7位func-code
    uint16_t mid = 0;
    switch (instruc->func) {
        case(0x33): //add
            instruc->funct3 = (instruction >> 12) & 0b111;
            if (instruc->funct3 == 0b000 && ((instruction >> 25) & 0x7F) == 0) {
                instruc->rd = (instruction >> 7) & 0b11111;
                instruc->rs1 = (instruction >> 15) & 0b11111;
                instruc->rs2 = (instruction >> 20) & 0b11111;
            }
        break;

        case(0b0010011): // addi
            instruc->rd = (instruction >> 7) & 0b11111;
            instruc->rs1 = (instruction >> 15) & 0b11111;
            instruc->imm = (instruction >> 20) & 0b111111111111;
            break;

        case(0b0110111): // lui
            instruc->rd = (instruction >> 7) & 0b11111;
            instruc->imm = (instruction >> 12) & 0b11111111111111111111;
            break;
        
            case(0b0000011): // lw or lbu
            instruc->funct3 = (instruction >> 12) & 0b111; // 提取funct3位
            switch(instruc->funct3) {
                case(0b010): // lw
                    instruc->rd = (instruction >> 7) & 0b11111;
                    instruc->rs1 = (instruction >> 15) & 0b11111;
                    instruc->imm = (instruction >> 20) & 0b111111111111;
                    break;
                case(0b100): // lbu
                    instruc->rd = (instruction >> 7) & 0b11111;
                    instruc->rs1 = (instruction >> 15) & 0b11111;
                    instruc->imm = (instruction >> 20) & 0b111111111111;
                    break;
            }
            break;

        
        case(0b0100011): // sw or sb
            instruc->funct3 = (instruction >> 12) & 0b111;  
            instruc->rs1 = (instruction >> 15) & 0b11111;
            instruc->rs2 = (instruction >> 20) & 0b11111;
            mid = ((instruction >> 25) << 5) | ((instruction >> 7) & 0x1F);    // ????
            instruc->imm = (uint32_t)mid;
        break;
        
        case(0b1100111): // jalr
            instruc->rd = (instruction >> 7) & 0b11111;
            instruc->rs1 = (instruction >> 15) & 0b11111;
            instruc->imm = (instruction >> 20) & 0b111111111111;
        break;

        case(0b1110011):        // ebreak

        
        break;
        }
    return instruc;
}

/*
    0-add -> func:0111011,rd,uw:000,rs1,rs2
    1-addi -> func:0010011,rd,uw:000,rs1,imm[11:0]
    2-lui -> func:0110111,rd,imm[31:12]
    3-lw -> func:0000011,rd,uw:010,rs1,imm[11:0]
    4-lbu -> func:0000011,rd,uw:100,rs1,imm[11:0]
    5-sw -> func:0100011,imm[4:0],uw:010,rs1,rs2,imm[11:5]
    6-sb -> func:0100011,imm[4:0],uw:000,rs1,rs2,imm[11:5]
    7-jalr -> func:1100111,rd,uw:000,rs1,imm[11:0]
*/