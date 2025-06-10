#ifndef __SYS_H__
#define __SYS_H__

#define __NR_syscalls 4

#ifndef __ASM__
#include <stdint.h>

void sys_write(char* buf);
int32_t sys_fork();

#endif

#endif // __SYS_H__
