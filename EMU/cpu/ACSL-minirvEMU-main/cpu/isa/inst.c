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
			uint32_t next_pc = cpu->pc + 4;\
			decode_operand(&cpu->decode, &rd, &rs1, &rs2, &imm); \
			__VA_ARGS__; \
			cpu->pc = next_pc;\
			goto *__match_end; \
		}\
	}while(0)

	MATCH_START();

	MATCH(OP_ADD,	R(rd) = R(rs1) + R(rs2));
	MATCH(OP_ADDI,	R(rd) = R(rs1) + imm);
	// TODO: add more instructions

	MATCH(OP_ERROR, log_err(ERR_UNKNOWN, "UNKNOWN instruction: %#08x", cpu->decode.inst), exit(1));

	MATCH_END();
	
	// TODO: You need to add some code to make the xxx.bin run correctly

}