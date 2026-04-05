#include "include/decode.h"

#define inst_key_length 32 + 5

#define INST(op, key, op_type) {OP_ ## op, key, 0, 0, OP_TYPE_ ## op_type}

Inst_t inst_table[] = {
	INST(ADD,    "0000000 ????? ????? 000 ????? 0110011", R),
	INST(ADDI,   "??????? ????? ????? 000 ????? 0010011", I),
	INST(LUI,    "??????? ????? ????? ??? ????? 0110111", U),
	INST(LW,     "??????? ????? ????? 010 ????? 0000011", I),
	INST(LBU,    "??????? ????? ????? 100 ????? 0000011", I),
	INST(SW,     "??????? ????? ????? 010 ????? 0100011", S),
	INST(SB,     "??????? ????? ????? 000 ????? 0100011", S),
	INST(JALR,   "??????? ????? ????? 000 ????? 1100111", I),

	INST(ERROR,  "??????? ????? ????? ??? ????? ???????", ERROR)
};

void Decode_Init(){
	for(size_t i = 0; i < sizeof(inst_table) / sizeof(Inst_t); i++){
		uint64_t key = 0, mask = 0;
		char *c = inst_table[i].inst_key;
		for(int j = 0; j < inst_key_length; j++, c++){
			if(*c != ' ') {
				key = (key << 1) | (*c == '1' ? 1 : 0);
				mask = (mask << 1) | (*c == '?' ? 0 : 1);
			}
		}
		inst_table[i].key = key;
		inst_table[i].mask = mask;
	}
}

void Pattern_match(Decode *decode){
	for(size_t i = 0; i < sizeof(inst_table) / sizeof(Inst_t); i++){
		if((decode->inst & inst_table[i].mask) == inst_table[i].key){
			decode->op = inst_table[i].op;
			decode->op_type = inst_table[i].op_type;
			return;
		}
	}
	// 如果没有匹配到任何指令，设置为 ERROR
	decode->op = OP_ERROR;
	decode->op_type = OP_TYPE_ERROR;
}

void Decode_OP(Decode *decode){
	Pattern_match(decode);
}

void decode_operand(Decode *decode, uint32_t *rd, uint32_t *rs1, uint32_t *rs2, uint32_t *imm){
	#define BITMASK(bits) ((1ull << (bits)) - 1)
	#define BITS(x, hi, lo) (((x) >> (lo)) & BITMASK((hi) - (lo) + 1))
	#define SEXT(x, len) ({ struct { int64_t n : len; } __x = { .n = x }; (uint64_t)__x.n; })

	#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
	#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
	#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)

	uint32_t i = decode->inst;
	*rs1 = BITS(i, 19, 15);
	*rs2 = BITS(i, 24, 20);
	*rd  = BITS(i, 11, 7);

	switch(decode->op_type){
		case OP_TYPE_I:
			immI();
			break;
		case OP_TYPE_U:
			immU();
			break;
		case OP_TYPE_S:
			immS();
			break;
		default:
			*imm = 0;
			break;
	}

	#undef BITMASK
	#undef BITS
	#undef SEXT
	#undef immI
	#undef immU
	#undef immS
}
