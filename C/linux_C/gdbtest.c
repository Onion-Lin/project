/*************************************************************************
    > File Name    : 10_1_gdbtest.c
    > Author       : SuperYSL
    > Email        : 324******4@qq.com
    > Created Time : 2026-04-04 21:46:45
 ************************************************************************/

#include<stdio.h>

int add_range(int low, int high){
	int i, sum = 0;
	for (i = low; i <= high; i++)
		sum = sum + i;
	return sum;
}

int main(int argc, char* argv[]){
	int result[100];
	result[0] = add_range(1, 10);
	result[1] = add_range(1, 100);
	printf("result[0]=%d\nresult[1]=%d\n", result[0], result[1]);
	return 0;
}
