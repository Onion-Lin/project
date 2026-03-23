#ifndef SIM_CPU_H
#define SIM_CPU_H 
#include <util.h>
#include "memory.h"

// Forward declaration of struct CPU
struct CPU;

#define Mr Mem_Struct_read
#define Mw Mem_Struct_write

void GPR_Init(struct CPU *cpu);
void CPU_Init(struct CPU *cpu);
void CPU_Fectch_Inst(struct CPU *cpu);
bool CPU_Decode_Inst(struct CPU *cpu);
void CPU_Exec_Inst(struct CPU *cpu);
uint32_t CPU_Read(struct CPU *cpu, uint32_t target);


#endif