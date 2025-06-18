#include "fork/fork.h"
#include "io/mmio.h"
#include "io/uart/printf.h"
#include "irq/irq.h"
#include "power/power.h"
#include "scheduler/scheduler.h"
#include "sd/fatfs/ff.h"
#include "syscalls/syscalls.h"
#include "system_timer/system_timer.h"
#include "user/user.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

extern uint32_t get_el();

void kernel_process() {
	printf("Kernel process started. EL %d\r\n", get_el());
	int err = move_to_user_mode((uint64_t)&user_process);
	if (err < 0) {
		printf("Error while moving process to user mode\n\r");
	}

	while (1);
}

void main() {
	uart_init();

	puts("EL: ");
	putchar(get_el() + '0');

	printf("\nCompilation time: " __DATE__ " " __TIME__ "\n");

	enable_interrupts();
	init_timer();

	int res = copy_process(PF_KTHREAD, (uint64_t)&kernel_process, (uint64_t)"12345", 0);
	if (res < 0) {
		printf("error while starting kernel process\n");
	}

	while (1);
}
