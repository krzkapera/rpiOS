#include "scheduler.h"
#include "../irq/entry.h"
#include "../mem/mem.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct* current = &(init_task);
struct task_struct* task[NR_TASKS] = {&(init_task)};

uint32_t nr_tasks = 1;

void preempt_disable() {
	current->preempt_count++;
}

void preempt_enable() {
	current->preempt_count--;
}

void _schedule() {
	preempt_disable();
	int next, c;
	struct task_struct* p;
	while (1) {
		c = -1;
		next = 0;
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p && p->state == TASK_RUNNING && p->counter > c) {
				c = p->counter;
				next = i;
			}
		}
		if (c) {
			break;
		}
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p) {
				p->counter = (p->counter >> 1) + p->priority;
			}
		}
	}
	printf("switch to %d\n", next);
	switch_to(task[next]);
	preempt_enable();
}

void schedule() {
	current->counter = 0;
	_schedule();
}

void switch_to(struct task_struct* next) {
	if (current == next)
		return;
	struct task_struct* prev = current;
	current = next;
	cpu_switch_to(prev, next);
}

void schedule_tail() {
	preempt_enable();
}

void timer_tick() {
	--current->counter;
	if (current->counter > 0 || current->preempt_count > 0) {
		return;
	}
	current->counter = 0;
	irq_enable();
	_schedule();
	irq_disable();
}

void exit_process() {
	preempt_disable();
	for (int i = 0; i < NR_TASKS; i++) {
		if (task[i] == current) {
			task[i]->state = TASK_ZOMBIE;
			break;
		}
	}
	if (current->stack) {
		free_page(current->stack);
	}
	preempt_enable();
	schedule();
}
