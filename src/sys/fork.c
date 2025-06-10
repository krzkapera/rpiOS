#include "fork.h"
#include "../io/uart/printf.h"
#include "../irq/entry.h"
#include "../mmu/mm.h"
#include "sched.h"
#include "sysregs.h"
#include <stdint.h>
#include <string.h>

void copy_ke_regs(struct ke_regs* to, struct ke_regs* from) {
	for (int i = 0; i < 31; i++) {
		to->regs[i] = from->regs[i];
	}
	to->sp = from->sp;
	to->elr = from->elr;
	to->pstate = from->pstate;
}

int copy_process(uint64_t clone_flags, uint64_t fn, uint64_t arg) {
	preempt_disable();
	struct task_struct* p;

	/* allocate page for new task's task_struct, ke_regs and kernel stack */
	p = (struct task_struct*)get_kernel_page();

	struct ke_regs* childregs = task_ke_regs(p);
	memzero((uint64_t)childregs, sizeof(struct ke_regs));
	memzero((uint64_t)&(p->core_context), sizeof(struct core_context));

	/* set x19 and x20 to run in ret_from_fork if we are creating a kernel thread */
	if (clone_flags & KTHREAD) {
		p->core_context.x19 = fn;
		p->core_context.x20 = arg;
	} else {
		struct ke_regs* cur_regs = task_ke_regs(current);
		/* copy the ke_regs from parent, this includes the elr */
		/* special struct copy function is used to avoid gcc generating a memcpy call */
		copy_ke_regs(childregs, cur_regs);
		/* set child's x0 after kernel_exit to 0 to indicate it is child */
		childregs->regs[0] = 0;
		if (copy_virt_memory(p) != 0) {
			free_kernel_page((uint64_t)p);
			return -1;
		}
	}

	p->flags = clone_flags;
	p->priority = current->priority;
	p->state = TASK_RUNNING;
	p->counter = p->priority;
	p->preempt_count = 1;

	p->core_context.lr = (uint64_t)ret_from_fork;
	p->core_context.sp = (uint64_t)childregs;

	int pid = nr_tasks++;
	task[pid] = p;
	printf("created pid %c\n", '0' + pid);
	preempt_enable();
	return pid;
}

int prepare_move_to_user(uint64_t start_addr, uint64_t size, uint64_t fn) {
	struct ke_regs* regs = task_ke_regs(current);
	memzero((uint64_t)regs, sizeof(*regs));
	/* jump to fn as EL0 after eret in kernel_exit */
	regs->elr = fn;
	regs->pstate = SPSR_EL1_MODE_EL0t;
	// regs->sp = USER_SP_INIT_POS;
	uint64_t code_page = allocate_user_page(current, 0);
	if (code_page == 0)
		return -1;
	regs->sp = code_page + PAGE_SIZE;
	memcpy((uint64_t*)code_page, (uint64_t*)start_addr, size);
	set_pgd(current->mm.pgd);
	return 0;
}

struct ke_regs* task_ke_regs(struct task_struct* tsk) {
	uint64_t p = (uint64_t)tsk + THREAD_SIZE - sizeof(struct ke_regs);
	return (struct ke_regs*)p;
}
