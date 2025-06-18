#include "../fork/fork.h"
#include "../io/mmio.h"
#include "../io/uart/printf.h"
#include "../syscalls/syscalls.h"
#include "../system_timer/system_timer.h"
#include <stdint.h>
#include <string.h>

void blink_led() {
	gpio_function(42, GPIO_FUNCTION_OUTPUT);
	mmio_write(GPSET1, 1 << 10);
	wait(2e6);
	mmio_write(GPCLR1, 1 << 10);
	wait(2e6);
}

void user_process1(char* array) {
	char buf[3] = {0, '\n', 0};
	while (1) {
		for (int i = 0; i < 5; i++) {
			buf[0] = array[i];
			call_sys_write(buf);
			wait(1e6);
		}
	}
}

void user_process2(char* array) {
	gpio_function(42, GPIO_FUNCTION_OUTPUT);
	char buf[3] = {0, '\n', 0};
	while (1) {
		for (int i = 0; i < 5; i++) {
			if (i % 2) {
				mmio_write(GPCLR1, 1 << 10);
			} else {
				mmio_write(GPSET1, 1 << 10);
			}

			buf[0] = array[i];
			call_sys_write(buf);

			wait(1e6);
		}
	}
}

void user_process() {
	char buf[30] = {0};
	memcpy(buf, "User process started\n", 22);
	call_sys_write(buf);
	uint64_t stack = call_sys_malloc();
	if (stack < 0) {
		printf("Error while allocating stack for process 1\n");
		return;
	}
	int err = call_sys_clone((uint64_t)&user_process1, (uint64_t)"12345", stack);
	if (err < 0) {
		printf("Error while clonning process 1\n");
		return;
	}
	stack = call_sys_malloc();
	if (stack < 0) {
		printf("Error while allocating stack for process 2\n");
		return;
	}
	err = call_sys_clone((uint64_t)&user_process2, (uint64_t)"abcde", stack);
	if (err < 0) {
		printf("Error while clonning process 2\n");
		return;
	}
	call_sys_exit();
}
