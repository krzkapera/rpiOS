#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "../io/uart/printf.h"
#include "../irq/irq.h"
#include "../mmu/pgd.h"
#include <stdint.h>

#define THREAD_CPU_CONTEXT 0
#define THREAD_SIZE 4096

#define NR_TASKS 64

#define FIRST_TASK task[0]
#define LAST_TASK task[NR_TASKS - 1]

#define TASK_RUNNING 0
#define TASK_ZOMBIE 1

#define PF_KTHREAD 0x00000002

#define INIT_TASK                                                                                  \
	{                                                                                              \
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 15, 0, PF_KTHREAD, {                        \
			0, 0, {{0}}, 0, {0}                                                                    \
		}                                                                                          \
	}

struct cpu_context {
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
	uint64_t pc;
};

#define MAX_PROCESS_PAGES 16

struct user_page {
	uint64_t phys_addr;
	uint64_t virt_addr;
};

struct mm_struct {
	uint64_t pgd;
	int32_t user_pages_count;
	struct user_page user_pages[MAX_PROCESS_PAGES];
	int kernel_pages_count;
	uint64_t kernel_pages[MAX_PROCESS_PAGES];
};

struct task_struct {
	struct cpu_context cpu_context;
	long state;
	long counter;
	long priority;
	long preempt_count;
	uint64_t flags;
	struct mm_struct mm;
};

extern struct task_struct* current;
extern struct task_struct* task[NR_TASKS];
extern uint32_t nr_tasks;

void schedule();
void timer_tick();
void preempt_disable();
void preempt_enable();
void switch_to(struct task_struct* next);
extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
void exit_process();

#endif // __SCHEDULER_H__
