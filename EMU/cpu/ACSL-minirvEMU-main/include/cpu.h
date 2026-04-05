#ifndef CPU_H
#define CPU_H
#include <util.h>
#include "../cpu/isa/include/decode.h"

#define GPR_NUM 16


typedef struct CPU{
	uint32_t reg[GPR_NUM];
	uint32_t pc;		// current pc
	uint32_t next_pc;	// next pc
	uint32_t circles;	// record the number of cicles
	Decode decode;		// store the decoded result

	void (*init)(struct CPU *cpu);			// init cpu
	void (*fetch_inst)(struct CPU *cpu);	// fetch instruction
	bool (*decode_inst)(struct CPU *cpu);	// decode instruction
	void (*exec_inst)(struct CPU *cpu);		// execute instruction and write back

	uint32_t (*read)(struct CPU *cpu, uint32_t target);		// read register or other by yourself, you can rewrite it
} CPU;

// declare the function that struct CPU need
void CPU_Struct_Init();
void CPU_Struct_Fetch_Inst();
bool CPU_Struct_Decode_Inst();
void CPU_Struct_Exec_Inst();
uint32_t CPU_Struct_Read(uint32_t target);
uint32_t CPU_Struct_Get_PC(void);

#endif