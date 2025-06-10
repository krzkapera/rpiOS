#ifndef FORK_H
#define FORK_H

#include "../scheduler/scheduler.h"

#define UTHREAD 0
#define KTHREAD 1

int copy_process(uint64_t clone_flags, uint64_t fn, uint64_t arg);
int prepare_move_to_user(uint64_t start_addr, uint64_t size, uint64_t fn);
struct ke_regs* task_ke_regs(struct task_struct* tsk);

/* kernel entry/exit regs */
struct ke_regs {
	uint64_t regs[31];
	uint64_t sp;
	uint64_t elr;
	uint64_t pstate;
};

#endif /* FORK_H */
