#include "system_loader.h"
#include "../io/uart/printf.h"
#include "../io/uart/uart.h"
#include "../power/power.h"
#include "../sd/fatfs/ff.h"
#include "../system_timer/system_timer.h"
#include <stdbool.h>
#include <string.h>

uint8_t calc_checksum();
void write_file();
void clear_buffer();
void update_system();

uint8_t buffer[BUFFER_SIZE] = {0};
uint32_t buffer_length = 0;
uint8_t expected_checksum;

uint32_t length = 0;
uint8_t length_checksum;

uint8_t started = 0;

void read_data() {
	if (!started) {
		puts("Update started\n");
		started = 1;
	}

	while (uart_is_read_byte_ready()) {
		uint8_t c = getchar();
		buffer[buffer_length] = c;
		buffer_length++;

		if (c == 0xAA) {
			if (buffer_length == 7) {
				length = *(uint32_t*)buffer;

				uint8_t got_length_checksum = buffer[0] ^ buffer[1] ^ buffer[2] ^ buffer[3];
				uint8_t expected_length_checksum = buffer[4];

				expected_checksum = buffer[5];

				if (got_length_checksum != expected_length_checksum) {
					puts("length checksum nok\n");
				}
			} else if (buffer_length > 7 && buffer_length == length) {
				if (calc_checksum() == expected_checksum) {
					puts("all checksum ok\n");
					update_system();
				} else {
					puts("all checksum nok\n");
					clear_buffer();
				}
			}
		}
	}
}

uint8_t calc_checksum() {
	uint8_t sum = 0;
	for (int i = 6; i < buffer_length; i++) {
		sum ^= buffer[i];
	}
	return sum;
}

void clear_buffer() {
	memset(buffer, 0, BUFFER_SIZE);
	buffer_length = 0;
	length = 0;
	started = 0;
	puts("Buffer cleared\n");
}

void update_system() {
	for (int i = 7; i < buffer_length - 1; i++) {
		buffer[i - 7] = buffer[i];
	}
	for (int i = 1; i <= 8; i++) {
		buffer[buffer_length - i] = 0;
	}
	buffer_length -= 8;

	write_file();
	puts("Rebooting...\n");
	wait(1e3);
	restart();
}

void write_file() {
	FATFS fs;
	FRESULT fr = f_mount(&fs, "", 1);
	if (fr != FR_OK) {
		printf("f_mount() failed, fr = %d\n", fr);
		while (1);
	}

	FIL file;
	UINT bytesWritten;

	fr = f_open(&file, "kernel8.img", FA_WRITE | FA_CREATE_ALWAYS);
	if (fr != FR_OK) {
		printf("Cannot open file: %d\n", fr);
		while (1);
	}

	fr = f_write(&file, buffer, buffer_length, &bytesWritten);
	if (fr != FR_OK || bytesWritten != buffer_length) {
		printf("Write error: %d\n", fr);
	} else {
		printf("%u bytes saved\n", bytesWritten);
	}

	f_close(&file);
}
