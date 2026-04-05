#include <cpu.h>
#include <sim_cpu.h>

static CPU simulate_cpu;

void CPU_Struct_Init(){
	simulate_cpu.init = CPU_Init;
	simulate_cpu.init(&simulate_cpu);
}

void CPU_Struct_Fetch_Inst(){
	simulate_cpu.fetch_inst(&simulate_cpu);
}

bool CPU_Struct_Decode_Inst(){
	return simulate_cpu.decode_inst(&simulate_cpu);
}

void CPU_Struct_Exec_Inst(){
	simulate_cpu.exec_inst(&simulate_cpu);
}

uint32_t CPU_Struct_Read(uint32_t target){
	return simulate_cpu.read(&simulate_cpu, target);
}

uint32_t CPU_Struct_Get_PC(void){
	return CPU_Get_PC(&simulate_cpu);
}