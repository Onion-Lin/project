#include <memory.h>
#include <sim_mem.h>

void mem_init(struct Memory *memory, char* img_file){
	memory->size = MEM_SIZE;
	memory->mem = malloc(memory->size);
	memory->read = mem_read;
	memory->write = mem_write;
	memory->free = mem_free;
	memory->load_img = load_img;
	memory->load_img(memory, img_file);
}

void mem_write(struct Memory *memory remove_me, uint32_t address remove_me, uint32_t value remove_me, uint8_t Axsize remove_me){
	// TODO:
	// you should accomplish this function by yourself
	// depending on the table below
	// Axsize		Meaning
	// 0b000	 write 1 byte
	// 0b001	 write 2 bytes
	// 0b010	 write 4 bytes

	TODO();
}
uint32_t mem_read(struct Memory *memory remove_me, uint32_t address remove_me, uint8_t Axsize remove_me){
	// TODO:
	// you should accomplish this function by yourself
	// the return value must be a uint32_t value

	return 0xdeadbeef;
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

