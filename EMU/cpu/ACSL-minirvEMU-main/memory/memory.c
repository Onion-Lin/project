#include <memory.h>
#include <sim_mem.h>

static Mem simulate_mem;

void Mem_Struct_Init(char* img_file){
	simulate_mem.init = mem_init;
	simulate_mem.init(&simulate_mem, img_file);
}

void Mem_Struct_write(uint32_t address, uint32_t value, uint8_t Axsize){
	simulate_mem.write(&simulate_mem, address, value, Axsize);
}

uint32_t Mem_Struct_read(uint32_t address, uint8_t Axsize){
	return simulate_mem.read(&simulate_mem, address, Axsize);
}

void Mem_Struct_free(){
	simulate_mem.free(&simulate_mem);
}
