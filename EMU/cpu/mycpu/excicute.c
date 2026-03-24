#include "cpu.h"

// 执行函数
void execute(ins* instruc) {
    check(instruc != NULL, "instruc pointer is NULL");
    check(instruc->func != 0, "Invalid opcode (marked as 0)");

    //变量定义
    uint32_t val1 = 0, val2 = 0, addr = 0, jump_addr = 0,check = 0;

    switch (instruc->func) {
        case 0x33:  // add
                val1 = gpr_r(instruc->rs1);
                val2 = gpr_r(instruc->rs2);
                gpr_w(instruc->rd, val1 + val2);
        break;

        case 0b0010011:  // addi
                val1 = gpr_r(instruc->rs1);
                gpr_w(instruc->rd, val1 + SEXT(instruc->imm12,12));
            break;

        case 0b0110111:  // lui
                gpr_w(instruc->rd, instruc->imm20 << 12); 
            break;
        
        case 0b0000011:{  // lw or lbu
            switch (instruc->funct3) {  
                case 0b010:         // lw               
                        uint32_t addr = SEXT(instruc->imm12,12) + gpr_r(instruc->rs1);
                        //连续读取四个字节并拼接
                        uint32_t val = (ram_read(addr + 3) << 24) | (ram_read(addr + 2) << 16) | (ram_read(addr + 1) << 8) | ram_read(addr);
                        gpr_w(instruc->rd, val);
                    break;

                case 0b100:{        // lbu
                        uint32_t addr = SEXT(instruc->imm12,12) + gpr_r(instruc->rs1);
                        //只取一个字节，零扩展到32位
                        uint32_t val = ram_read(addr) & 0xFF;  
                        gpr_w(instruc->rd, val);
                    break;

                default:
                        log_warn("Unknown funct3 for load: 0x%01X", instruc->funct3);
                    break;
                }  
            }
            break;
        }
        case 0b0100011:{  // sw or sb
            switch (instruc->funct3) { 
                case 0b010:{  // sw
                    uint32_t addr = instruc->imm12 + gpr_r(instruc->rs1);
                    //uint32_t addr = SEXT(instruc->imm12,12) + gpr_r(instruc->rs1);
                    uint32_t val = gpr_r(instruc->rs2);

                    ram_write(addr, (val ) & 0xFF);
                    ram_write(addr + 1, (val >> 8) & 0xFF);
                    ram_write(addr + 2, (val >> 16) & 0xFF);
                    ram_write(addr + 3, (val >> 24) & 0xFF);
                }
                break;

                case 0b000:{  // sb
                
                    uint32_t addr = SEXT(instruc->imm12,12) + gpr_r(instruc->rs1);
                    uint8_t val = gpr_r(instruc->rs2) & 0xFF;
                    ram_write(addr, val);
                }
                break;

                default:
                    log_warn("Unknown funct3 for store: 0x%01X", instruc->funct3);
                    break;
            }
            break;
        }


        case 0b1100111: { // jalr
            //指令的执行是有顺序的，先计算跳转地址，再保存返回地址，最后更新PC
            instruc->addr = (GPR[instruc->rs1] + SEXT(instruc->imm12,12)) & ~1;  // 计算跳转地址
            gpr_w(instruc->rd, PC + 4);  // 保存返回地址
            //pc_add(1,instruc->addr);
            break;
        }


        case(0b1110011):        // ebreak
            check = gpr_r(10);  // 读取a0寄存器的值判断读取是否正确
            log_info("EBREAK encountered, a0=%u", check);
            check(check == 0x0, "EBREAK check failed", check);

            exit(check);
        break;

        default:
            log_warn("Unsupported instruction func: 0x%02X", instruc->func);
            break;
    }
    return;

error:
    return;
}