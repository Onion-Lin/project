#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define ANSI_FG_RED     "\33[1;31m"
#define ANSI_FG_GREEN   "\33[1;32m"
#define ANSI_FG_YELLOW  "\33[1;33m"
#define ANSI_FG_BLUE    "\33[1;34m"
#define ANSI_NONE       "\33[0m"

enum Err{
	ERR_NONE = 0,
	ERR_INIT,
	// TODO: add more error types which you need
	ERR_TRAP,
	ERR_TODO,
	ERR_UNKNOWN,
};

char* ErrToString(enum Err err);

#define remove_me __attribute__((unused))


#ifdef log_err
#undef log_err
#define log_err(Err, M, ...) \
	fprintf(stderr, ANSI_FG_RED "[ERROR] " ANSI_NONE "(%s:%d: errno: %s)" M "\n", __FILE__, __LINE__, ErrToString(Err), ##__VA_ARGS__)
#else
#define log_err(Err, M, ...) \
	fprintf(stderr, ANSI_FG_RED "[ERROR] " ANSI_NONE "(%s:%d: errno: %s)" M "\n", __FILE__, __LINE__, ErrToString(Err), ##__VA_ARGS__)
#endif

#define log_success(M, ...) \
	printf(ANSI_FG_GREEN "[SUCCESS] " ANSI_NONE M "\n", ##__VA_ARGS__)

#define TODO() do{\
	extern void my_free(); \
	log_err(ERR_TODO, "This must be implemented by your self"); \
	my_free(); \
	exit(ERR_TODO); \
}while(0)

#endif