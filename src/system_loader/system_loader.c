#include "system_loader.h"
#include "../io/system_timer/system_timer.h"
#include "../io/uart/printf.h"
#include "../io/uart/uart.h"
#include "../power/power.h"
#include "../sd/fatfs/ff.h"
#include <stdbool.h>

void write_file();

uint8_t buffer[BUFFER_SIZE] = {0};
int buffer_length = 0;
uint8_t expected_checksum;

uint8_t calc_checksum() {
	uint8_t sum = 0;
	for (int i = 0; i < buffer_length; i++) {
		sum ^= buffer[i];
	}
	return sum;
}

void read_data() {
	while (uart_is_read_byte_ready()) {
		uint8_t c = getchar();
		buffer[buffer_length] = c;
		buffer_length++;

		if (c == 1 && buffer_length % 2 == 0 && buffer_length >= MIN_IMG_SIZE) {
			expected_checksum = buffer[buffer_length - 2];
			buffer[buffer_length - 2] = 0;
			buffer[buffer_length - 1] = 0;

			for (int i = 2; i < buffer_length; i += 2) {
				buffer[i / 2] = buffer[i];
				buffer[i] = 0;
			}

			buffer_length -= 2;
			buffer_length /= 2;

			uint8_t got_checksum = calc_checksum();

			printf("\n%d %d %d\n", buffer_length, expected_checksum, got_checksum);
			if (expected_checksum == got_checksum) {
				write_file();
				puts("Rebooting...\n");
				wait(1e3);
				restart();
			}
		}
	}
}

void write_file() {
	FATFS fs;
	FRESULT fr = f_mount(&fs, "", 1);
	if (fr != FR_OK) {
		printf("f_mount() failed, fr = %d\n", fr);
		while (1)
			;
	}

	FIL file;
	UINT bytesWritten;

	fr = f_open(&file, "kernel8.img", FA_WRITE | FA_CREATE_ALWAYS);
	if (fr != FR_OK) {
		printf("Cannot open file: %d\n", fr);
		while (1)
			;
	}

	fr = f_write(&file, buffer, buffer_length, &bytesWritten);
	if (fr != FR_OK || bytesWritten != buffer_length) {
		printf("Write error: %d\n", fr);
	} else {
		printf("%u bytes saved\n", bytesWritten);
	}

	f_close(&file);
}
