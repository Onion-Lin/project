#include <cpu.h>
#include <memory.h>
#include <util.h>
#include <vga_display.h>

#define MAX_CIRCLES 100000000

bool init(char* img_file);
void my_free();
int start(int argc, char **argv);
char* Arg_img_file(int argc, char **argv);

static inline void execOnce() {
	uint32_t pc_before = CPU_Struct_Get_PC();
	CPU_Struct_Fetch_Inst();
	CPU_Struct_Decode_Inst();
	CPU_Struct_Exec_Inst();
	uint32_t pc_after = CPU_Struct_Get_PC();
	
	// 调试：打印前 100 次执行的指令
	static int debug_count = 0;
	if (debug_count < 100) {
		printf("Exec %d: PC 0x%08X -> 0x%08X\n", debug_count, pc_before, pc_after);
		debug_count++;
	}
}

int main(int argc, char **argv) {
	// 初始化 VGA 显示
	vga_display_init();

	// init
	if(init(Arg_img_file(argc, argv)) != true){
		log_err(ERR_INIT, "init failed");
	}

	for(int i = 0; i < MAX_CIRCLES; i++){
		execOnce();
		// 检测 halt（PC 被设置为 0xFFFFFFFF）
		if (CPU_Struct_Get_PC() == 0xFFFFFFFF) {
			break;
		}
		// 调试：打印 PC 值
		if (i % 100000 == 0) {
			printf("Circle %d, PC=0x%08X\n", i, CPU_Struct_Get_PC());
		}
	}

	// 刷新显示
	vga_display_refresh();

	// 等待用户关闭窗口
	vga_display_wait();

	my_free();

	// 清理 VGA 显示资源
	vga_display_cleanup();

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
