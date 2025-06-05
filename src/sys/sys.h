#ifndef __SYS_H__
#define __SYS_H__

#include <stdint.h>

#define __NR_syscalls 4

#define SYS_WRITE_NUMBER 0
#define SYS_MALLOC_NUMBER 1
#define SYS_CLONE_NUMBER 2
#define SYS_EXIT_NUMBER 3

void sys_write(char* buf);
int sys_fork();
void call_sys_write(char* buf);
int call_sys_clone(uint64_t fn, uint64_t arg, uint64_t stack);
uint64_t call_sys_malloc();
void call_sys_exit();

#endif // __SYS_H__
