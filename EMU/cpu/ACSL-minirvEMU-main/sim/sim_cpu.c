#include <cpu.h>
#include <sim_cpu.h>
#include <inst.h>
#include <memory.h>

inline void GPR_Init(struct CPU *cpu){
	for (int i = 0; i < GPR_NUM; i++){
		cpu->reg[i] = 0;
	}
}

void CPU_Init(struct CPU *cpu){
	GPR_Init(cpu);
	Decode_Init();
	cpu->pc = MEM_BASE;
	cpu->next_pc = cpu->pc;
	cpu->circles = 0;
	cpu->decode = (Decode){.inst = 0, .op = OP_ERROR, .op_type = OP_TYPE_ERROR};
	cpu->fetch_inst = CPU_Fectch_Inst;
	cpu->decode_inst = CPU_Decode_Inst;
	cpu->exec_inst = CPU_Exec_Inst;
	cpu->read = CPU_Read;
}

void CPU_Fectch_Inst(struct CPU *cpu){
	// 从内存中读取 4 字节指令（小端序）
	uint32_t inst = Mem_Struct_read(cpu->pc, 0b010);
	cpu->decode.inst = inst;
	cpu->circles++;
}

bool CPU_Decode_Inst(struct CPU *cpu){
	Decode_OP(&cpu->decode);
	return cpu->decode.op != OP_ERROR;
}

void CPU_Exec_Inst(struct CPU *cpu){
	Inst_Exec(cpu);
}

uint32_t CPU_Read(struct CPU *cpu, uint32_t target){
	if(target < GPR_NUM){
		return cpu->reg[target];
	}else {
		log_err(ERR_UNKNOWN, "Invalid GPR number: %d", target);
		return 0;
	}
}

uint32_t CPU_Get_PC(struct CPU *cpu){
	return cpu->pc;
}
