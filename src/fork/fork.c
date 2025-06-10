#include "fork.h"
#include "../io/uart/printf.h"
#include "../irq/entry.h"
#include "../mem/mem.h"
#include <string.h>

extern void ret_from_fork();

int copy_process(uint64_t clone_flags, uint64_t fn, uint64_t arg, uint64_t stack) {
	preempt_disable();
	struct task_struct* p;

	p = (struct task_struct*)get_free_page();
	if (!p) {
		return -1;
	}

	struct pt_regs* childregs = task_pt_regs(p);
	memset(childregs, 0, sizeof(struct pt_regs));
	memset(&p->cpu_context, 0, sizeof(struct cpu_context));

	if (clone_flags & PF_KTHREAD) {
		p->cpu_context.x19 = fn;
		p->cpu_context.x20 = arg;
	} else {
		struct pt_regs* cur_regs = task_pt_regs(current);
		*childregs = *cur_regs;
		childregs->regs[0] = 0;
		childregs->sp = stack + PAGE_SIZE;
		p->stack = stack;
	}
	p->flags = clone_flags;
	p->priority = current->priority;
	p->state = TASK_RUNNING;
	p->counter = p->priority;
	p->preempt_count = 1; // disable preemtion until schedule_tail

	p->cpu_context.pc = (uint64_t)ret_from_fork;
	p->cpu_context.sp = (uint64_t)childregs;
	int pid = nr_tasks++;
	task[pid] = p;
	preempt_enable();
	return pid;
}

int move_to_user_mode(uint64_t pc) {
	struct pt_regs* regs = task_pt_regs(current);
	memset(regs, 0, sizeof(*regs));
	regs->pc = pc;
	regs->pstate = PSR_MODE_EL0t;
	uint64_t stack = get_free_page(); // allocate new user stack
	if (!stack) {
		return -1;
	}
	regs->sp = stack + PAGE_SIZE;
	current->stack = stack;
	return 0;
}

struct pt_regs* task_pt_regs(struct task_struct* tsk) {
	uint64_t p = (uint64_t)tsk + THREAD_SIZE - sizeof(struct pt_regs);
	return (struct pt_regs*)p;
}
