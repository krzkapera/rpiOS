#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "../irq/irq.h"
#include <stdint.h>

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

struct task_struct {
	struct cpu_context cpu_context;
	uint64_t state;
	uint64_t counter;
	uint64_t priority;
	uint64_t preempt_count;
};

extern void ret_from_fork();
uint64_t get_free_page();
void free_page(uint64_t p);

#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define LOW_MEMORY (2 * SECTION_SIZE)
#define HIGH_MEMORY 0x20000000

#define PAGING_MEMORY (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES (PAGING_MEMORY / PAGE_SIZE)

#define THREAD_CPU_CONTEXT 0
#define THREAD_SIZE 4096

#define NR_TASKS 64

#define FIRST_TASK task[0]
#define LAST_TASK task[NR_TASKS - 1]

#define TASK_RUNNING 0

#define INIT_TASK {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 1, 0}

extern struct task_struct* current;
extern struct task_struct* task[NR_TASKS];
extern int nr_tasks;

extern void sched_init(void);
extern void schedule(void);
extern void timer_tick(void);
extern void preempt_disable(void);
extern void preempt_enable(void);
extern void switch_to(struct task_struct* next);
extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);

#endif // __SCHEDULER_H__
