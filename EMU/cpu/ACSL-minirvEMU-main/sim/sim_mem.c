#include <memory.h>
#include <sim_mem.h>
#include <string.h>
#include <vga_display.h>

// VGA 显存缓冲区
static uint8_t vga_mem[VGA_SIZE];
static uint32_t vga_write_debug_count = 0;

void mem_init(struct Memory *memory, char* img_file){
	memory->size = MEM_SIZE;
	memory->mem = malloc(memory->size);
	if (memory->mem == NULL) {
		log_err(ERR_INIT, "Failed to allocate memory\n");
		return;
	}
	memset(memory->mem, 0, memory->size);
	memset(vga_mem, 0, VGA_SIZE);
	memory->read = mem_read;
	memory->write = mem_write;
	memory->free = mem_free;
	memory->load_img = load_img;
	memory->load_img(memory, img_file);
}

void mem_write(struct Memory *memory, uint32_t address, uint32_t value, uint8_t Axsize){
	// 检查是否为 VGA 内存映射地址
	if (address >= VGA_BASE && address < VGA_BASE + VGA_SIZE) {
		uint32_t offset = address - VGA_BASE;
		uint32_t write_size = (Axsize == 0b000) ? 1 : (Axsize == 0b001) ? 2 : 4;

		if (offset + write_size > VGA_SIZE) {
			return;
		}

		vga_write_debug_count++;
		if (vga_write_debug_count <= 10) {
			printf("[VGA WRITE] addr=0x%08X, offset=0x%06X, val=0x%08X, size=%d\n",
			       address, offset, value, write_size);
		}

		switch(Axsize) {
			case 0b000: // write 1 byte
				vga_mem[offset] = value & 0xFF;
				break;
			case 0b001: // write 2 bytes (halfword)
				vga_mem[offset]     = value & 0xFF;
				vga_mem[offset + 1] = (value >> 8) & 0xFF;
				break;
			case 0b010: // write 4 bytes (word)
				vga_mem[offset]     = value & 0xFF;
				vga_mem[offset + 1] = (value >> 8) & 0xFF;
				vga_mem[offset + 2] = (value >> 16) & 0xFF;
				vga_mem[offset + 3] = (value >> 24) & 0xFF;
				break;
		}

		// 更新显示
		vga_display_update(vga_mem, address, write_size);
		return;
	}

	// 普通内存写入
	if (address + Axsize > memory->size) {
		return;
	}

	switch(Axsize) {
		case 0b000: // write 1 byte
			memory->mem[address] = value & 0xFF;
			break;
		case 0b001: // write 2 bytes (halfword)
			memory->mem[address]     = value & 0xFF;
			memory->mem[address + 1] = (value >> 8) & 0xFF;
			break;
		case 0b010: // write 4 bytes (word)
			memory->mem[address]     = value & 0xFF;
			memory->mem[address + 1] = (value >> 8) & 0xFF;
			memory->mem[address + 2] = (value >> 16) & 0xFF;
			memory->mem[address + 3] = (value >> 24) & 0xFF;
			break;
		default:
			break;
	}
}

uint32_t mem_read(struct Memory *memory, uint32_t address, uint8_t Axsize){
	// 检查是否为 VGA 内存映射地址
	if (address >= VGA_BASE && address < VGA_BASE + VGA_SIZE) {
		uint32_t offset = address - VGA_BASE;
		uint32_t read_size = (Axsize == 0b000) ? 1 : (Axsize == 0b001) ? 2 : 4;

		if (offset + read_size > VGA_SIZE) {
			return 0;
		}

		uint32_t value = 0;
		switch(Axsize) {
			case 0b000: // read 1 byte
				value = vga_mem[offset] & 0xFF;
				break;
			case 0b001: // read 2 bytes (halfword)
				value = (vga_mem[offset] & 0xFF) |
				        ((vga_mem[offset + 1] & 0xFF) << 8);
				break;
			case 0b010: // read 4 bytes (word)
				value = (vga_mem[offset] & 0xFF) |
				        ((vga_mem[offset + 1] & 0xFF) << 8) |
				        ((vga_mem[offset + 2] & 0xFF) << 16) |
				        ((vga_mem[offset + 3] & 0xFF) << 24);
				break;
		}
		return value;
	}

	// 普通内存读取
	if (address + Axsize > memory->size) {
		return 0;
	}

	uint32_t value = 0;
	switch(Axsize) {
		case 0b000: // read 1 byte
			value = memory->mem[address] & 0xFF;
			break;
		case 0b001: // read 2 bytes (halfword)
			value = (memory->mem[address] & 0xFF) |
			        ((memory->mem[address + 1] & 0xFF) << 8);
			break;
		case 0b010: // read 4 bytes (word)
			value = (memory->mem[address] & 0xFF) |
			        ((memory->mem[address + 1] & 0xFF) << 8) |
			        ((memory->mem[address + 2] & 0xFF) << 16) |
			        ((memory->mem[address + 3] & 0xFF) << 24);
			break;
		default:
			break;
	}
	return value;
}

void mem_free(struct Memory *memory){
	if(memory->mem){
		free(memory->mem);
		memory->mem = NULL;
		printf("Memory free\n");
	}
}

bool load_img(struct Memory *memory, char* img_file){
	if(img_file != NULL){
		FILE* fp = fopen(img_file, "rb");
		long size;

		if(fp != NULL){
			fseek(fp, 0, SEEK_END);
			size = ftell(fp);
			fseek(fp, 0, SEEK_SET);
		}else {
			log_err(ERR_INIT, "Can't open image file\n");
			return false;
		}

		if(size > memory->size){
			log_err(ERR_INIT, "Image file is too large to load into memory\n");
			fclose(fp);
			return false;
		}else {
			fread(memory->mem, size, 1, fp);
			printf("Loaded image file %s. Size:%ld\n", img_file, size);
			return true;
		}

		fclose(fp);
	}else {
		log_err(ERR_INIT, "No image file specified\n");
	}
	return false;
}
