#ifndef _DECODE_
#define _DECODE_
#include <util.h>

enum OP{
	OP_ERROR,
	OP_ADD, OP_ADDI, OP_LUI, OP_LW,
	OP_LBU, OP_SW, OP_SB, OP_JALR
};

enum OP_TYPE{
	OP_TYPE_ERROR,
	OP_TYPE_R, OP_TYPE_I, OP_TYPE_S, OP_TYPE_B, OP_TYPE_U, OP_TYPE_J
};

typedef struct Inst{
	enum OP op;
	char *inst_key;
	uint32_t key;
	uint32_t mask;
	enum OP_TYPE op_type;
}Inst_t;

typedef struct Decode{
	uint32_t inst;
	enum OP op;
	enum OP_TYPE op_type;
} Decode;

void Decode_Init();
void Pattern_match(Decode *decode);
// 为decode设置op的值
void Decode_OP(Decode *decode);
void decode_operand(Decode *decode, uint32_t *rd, uint32_t *rs1, uint32_t *rs2, uint32_t *imm);

#endif