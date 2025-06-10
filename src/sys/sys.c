#include "../io/uart/printf.h"
#include "../scheduler/scheduler.h"
#include "./fork.h"

void sys_write(char* buf) {
	printf(buf);
}

int32_t sys_fork() {
	return copy_process(0, 0, 0);
}

void sys_exit() {
	exit_process();
}

void* const sys_call_table[] = {sys_write, sys_fork, sys_exit};
