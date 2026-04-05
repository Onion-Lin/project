#include <cpu.h>
#include <decode.h>
#include "include/inst.h"
#include <memory.h>

#define concat_unfold(x, y) x ## y
#define concat(x, y) concat_unfold(x, y)

void Inst_Exec(struct CPU* cpu){
	#define Mw Mem_Struct_write
	#define Mr Mem_Struct_read
	#define R(x) cpu->reg[x]
	#define MATCH_START(name) { const void * __match_end = && concat(match_end_, name);
	#define MATCH_END(name) concat(match_end_ , name): ; }
	#define MATCH(OP, ...) do { \
		if(OP == cpu->decode.op) { \
			uint32_t rd=0, rs1=0, rs2=0, imm=0; \
			uint32_t next_pc = cpu->pc + 4; \
			decode_operand(&cpu->decode, &rd, &rs1, &rs2, &imm); \
			__VA_ARGS__; \
			cpu->pc = next_pc; \
			goto *__match_end; \
		} \
	}while(0)

	MATCH_START();

	// R 型指令
	MATCH(OP_ADD,   R(rd) = R(rs1) + R(rs2));

	// I 型指令
	MATCH(OP_ADDI,  R(rd) = R(rs1) + imm);
	MATCH(OP_LUI,   R(rd) = imm);

	MATCH(OP_LW, {
		uint32_t addr = R(rs1) + imm;
		R(rd) = Mr(addr, 0b010);  // 读取 4 字节
	});

	MATCH(OP_LBU, {
		uint32_t addr = R(rs1) + imm;
		R(rd) = Mr(addr, 0b000);  // 读取 1 字节，零扩展
	});

	MATCH(OP_JALR, {
		uint32_t target = (R(rs1) + imm) & ~1;  // 最低位清零
		R(rd) = cpu->pc + 4;
		next_pc = target;
	});

	// S 型指令
	MATCH(OP_SW, {
		uint32_t addr = R(rs1) + imm;
		Mw(addr, R(rs2), 0b010);  // 写入 4 字节
	});

	MATCH(OP_SB, {
		uint32_t addr = R(rs1) + imm;
		Mw(addr, R(rs2), 0b000);  // 写入 1 字节
	});

	// 未知指令
	MATCH(OP_ERROR, {
		if (cpu->decode.inst == 0) {
			// 零指令视为 halt，设置 pc 为特殊值让主循环退出
			printf("[HALT] Zero instruction encountered at PC=0x%08x\n", cpu->pc);
			cpu->pc = 0xFFFFFFFF;  // 设置特殊 PC 值，让主循环检测到并退出
			next_pc = 0xFFFFFFFF;
		} else {
			log_err(ERR_UNKNOWN, "UNKNOWN instruction: %#08x at PC=0x%08x", cpu->decode.inst, cpu->pc);
			exit(1);
		}
	});

	MATCH_END();
}
