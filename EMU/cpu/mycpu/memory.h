#ifndef MEMORY_H
#define MEMORY_H

#define ROM_SIZE (1024 * 2048)  // ROM大小2MB
#define RAM_SIZE (1024 * 2048)  // RAM大小2MB

// 元件指针声明
extern uint8_t* rom;    // ROM指针（2MB）
extern uint8_t* ram;    // RAM指针（2MB）

// 函数声明（修正语法、统一命名风格）
uint32_t rom_fetch(uint32_t PC);              
uint8_t* rom_init();                             
uint8_t* ram_init();                             
uint8_t ram_read(uint32_t addr);                
void ram_write(uint32_t addr, uint8_t data);    
void ram_free();    
void rom_free();   

#endif