#include "fork.h"
#include "../io/uart/printf.h"
#include "../irq/entry.h"
#include "../mmu/mm.h"
#include "../scheduler/scheduler.h"
#include <string.h>

extern void ret_from_fork();

int32_t copy_process(uint64_t clone_flags, uint64_t fn, uint64_t arg) {
	preempt_disable();
	struct task_struct* p;

	uint64_t page = allocate_kernel_page();
	p = (struct task_struct*)page;
	struct pt_regs* childregs = task_pt_regs(p);

	if (!p)
		return -1;

	if (clone_flags & PF_KTHREAD) {
		p->cpu_context.x19 = fn;
		p->cpu_context.x20 = arg;
	} else {
		struct pt_regs* cur_regs = task_pt_regs(current);
		*childregs = *cur_regs;
		childregs->regs[0] = 0;
		copy_virt_memory(p);
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

int32_t move_to_user_mode(uint64_t start, uint64_t size, uint64_t pc) {
	struct pt_regs* regs = task_pt_regs(current);
	regs->pstate = PSR_MODE_EL0t;
	regs->pc = pc;
	regs->sp = 2 * PAGE_SIZE;
	uint64_t code_page = allocate_user_page(current, 0);
	if (code_page == 0) {
		return -1;
	}
	memcpy((void*)code_page, (void*)start, size);
	set_pgd(current->mm.pgd);
	return 0;
}

struct pt_regs* task_pt_regs(struct task_struct* tsk) {
	uint64_t p = (uint64_t)tsk + THREAD_SIZE - sizeof(struct pt_regs);
	return (struct pt_regs*)p;
}
