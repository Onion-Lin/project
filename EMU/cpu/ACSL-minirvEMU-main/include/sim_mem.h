#ifndef SIM_MEM_H
#define SIM_MEM_H
#include <util.h>

// Forward declaration of struct Memory
struct Memory;

void mem_init(struct Memory *memory, char* img_file);
void mem_write(struct Memory *memory, uint32_t address, uint32_t value, uint8_t Axsize);
uint32_t mem_read(struct Memory *memory, uint32_t address, uint8_t Axsize);
void mem_free(struct Memory *memory);
bool load_img(struct Memory *memory, char* img_file);

#endif