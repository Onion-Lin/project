#include <util.h>

char* ErrToString(enum Err err){
	switch (err) {
		case ERR_INIT:
			return "Init Error";
		case ERR_TRAP:
			return "Trap Error";
		case ERR_NONE:
			return "";
		case ERR_TODO:
			return "TODO";
		case ERR_UNKNOWN:
		default:
			return "Unknown: program meet a unknown behavior";
	}
}