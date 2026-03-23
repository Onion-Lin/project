#ifndef MEMORY_H
#define MEMORY_H
#include <util.h>

#define MEM_BASE 0
#define MEM_SIZE 0x8000000

typedef struct Memory {
	uint8_t *mem;
	uint32_t size;
	
	void (*init)(struct Memory *memory, char* img_file);
	void (*write)(struct Memory *memory, uint32_t address, uint32_t value, uint8_t Axsize);
	uint32_t (*read)(struct Memory *memory, uint32_t address, uint8_t Axsize);
	void (*free)(struct Memory *memory);
	bool (*load_img)(struct Memory *memory, char* img_file);
} Mem;

void Mem_Struct_Init(char* img_file);
void Mem_Struct_write(uint32_t address, uint32_t value, uint8_t Axsize);
uint32_t Mem_Struct_read(uint32_t address, uint8_t Axsize);
void Mem_Struct_free();
#endif