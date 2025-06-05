#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "../io/uart/printf.h"
#include "../irq/irq.h"
#include <stdint.h>

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
#define TASK_ZOMBIE 1

#define PF_KTHREAD 0x00000002

#define INIT_TASK {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 1, 0, 0, PF_KTHREAD}

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
	int64_t state;
	int64_t counter;
	int64_t priority;
	int64_t preempt_count;
	uint64_t stack;
	uint64_t flags;
};

extern struct task_struct* current;
extern struct task_struct* task[NR_TASKS];
extern uint32_t nr_tasks;

uint64_t get_free_page();
void free_page(uint64_t p);
void schedule();
void timer_tick();
void preempt_disable();
void preempt_enable();
void switch_to(struct task_struct* next);
extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
void exit_process();

#endif // __SCHEDULER_H__
