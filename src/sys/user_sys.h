#ifndef __USER_SYS_H__
#define __USER_SYS_H__

void call_sys_write(char* buf);
int call_sys_fork();
void call_sys_exit();

extern void user_delay(unsigned long);

#endif // __USER_SYS_H__
