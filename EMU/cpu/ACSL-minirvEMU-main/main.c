#include <cpu.h>
#include <memory.h>
#include <util.h>


// TODO: Replace with the correct value
#define MAX_CIRCLES 8

bool init(char* img_file);
void my_free();
int start(int argc, char **argv);
char* Arg_img_file(int argc, char **argv);

static inline void execOnce() {
	CPU_Struct_Fetch_Inst();
	CPU_Struct_Decode_Inst();
	CPU_Struct_Exec_Inst();
}

int main(int argc, char **argv) {
	// init
	if(init(Arg_img_file(argc, argv)) != true){
		log_err(ERR_INIT, "init failed");
	}

	for(int i = 0; i < MAX_CIRCLES; i++){
		execOnce();
	}

	if(CPU_Struct_Read(10) != 0){
		log_err(ERR_TRAP, "Hit bad trap");
	}else {
		log_success("simulate complete");
	}

	my_free();

	return 0;
}

bool init(char* img_file) {
	Mem_Struct_Init(img_file);
	CPU_Struct_Init();

	return true;
}

char *Arg_img_file(int argc, char **argv) {
	char *img_file = NULL;
	if(argc == 2) {
		img_file = argv[1];
	}
	return img_file;
}

void my_free() {	
	Mem_Struct_free();
}