#ifndef __FORK_H__
#define __FORK_H__

#include "../scheduler/scheduler.h"
#include <stdint.h>

#define PSR_MODE_EL0t 0x00000000
#define PSR_MODE_EL1t 0x00000004
#define PSR_MODE_EL1h 0x00000005
#define PSR_MODE_EL2t 0x00000008
#define PSR_MODE_EL2h 0x00000009
#define PSR_MODE_EL3t 0x0000000c
#define PSR_MODE_EL3h 0x0000000d

int copy_process(uint64_t clone_flags, uint64_t fn, uint64_t arg, uint64_t stack);
int move_to_user_mode(uint64_t pc);
struct pt_regs* task_pt_regs(struct task_struct* tsk);

struct pt_regs {
	uint64_t regs[31];
	uint64_t sp;
	uint64_t pc;
	uint64_t pstate;
};

#endif // __FORK_H__
