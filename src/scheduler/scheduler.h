#ifndef SCHED_H
#define SCHED_H

#define CORE_CONTEXT_OFFSET 0

#ifndef __ASSEMBLER__

#include <stdint.h>

#define NR_TASKS 64
#define TASK_RUNNING 0
#define TASK_ZOMBIE 1

#define MAX_PAGE_COUNT 16

extern struct task_struct* current;
extern struct task_struct* task[NR_TASKS];
extern int nr_tasks;

struct core_context {
	uint64_t x19;
	uint64_t x20;
	uint64_t x21;
	uint64_t x22;
	uint64_t x23;
	uint64_t x24;
	uint64_t x25;
	uint64_t x26;
	uint64_t x27;
	uint64_t x28;
	uint64_t fp;
	uint64_t sp;
	uint64_t lr;
};

struct user_page {
	uint64_t pa;
	uint64_t uva;
};

struct mm_struct {
	uint64_t pgd;
	struct user_page user_pages[MAX_PAGE_COUNT];
	uint64_t kernel_pages[MAX_PAGE_COUNT];
};

struct task_struct {
	struct core_context core_context;
	long state;
	long counter;
	long priority;
	long preempt_count;
	uint64_t flags;
	struct mm_struct mm;
};

void core_switch_to(struct task_struct* prev, struct task_struct* next);
void set_pgd(uint64_t pgd);
void preempt_disable();
void preempt_enable();
void _schedule();
void schedule();
void switch_to(struct task_struct* next);
void timer_tick();
void exit_process();
void sched_init();

#define INIT_TASK                                                                                  \
	/* core_context */ {                                                                           \
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* state, counter, priority, preempt_count */ 0,  \
			0, 1, 0, /* flags */ KTHREAD, /* mm */ {                                               \
			0, {{0}}, {0}                                                                          \
		}                                                                                          \
	}

#endif /* __ASSEMBLER__ */
#endif /* SCHED_H */
