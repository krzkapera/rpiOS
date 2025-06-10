#include "sys.h"
#include "../io/uart/printf.h"
#include "../mmu/mm.h"
#include "../scheduler/scheduler.h"
#include "fork.h"

void* sys_call_table[] = {sys_write, sys_fork, sys_exit};

void sys_write(char* buf) {
	puts(buf);
}

int sys_fork(uint64_t stack) {
	return copy_process(UTHREAD, 0, 0);
}

void sys_exit() {
	exit_process();
}

void sys_call_table_relocate() {
	for (int i = 0; i < NR_SYSCALLS; i++) {
		sys_call_table[i] = (void*)((uint64_t)sys_call_table[i] + (uint64_t)KERNEL_START);
	}
}
