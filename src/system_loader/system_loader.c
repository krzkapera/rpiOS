#include "system_loader.h"
#include "../io/uart/printf.h"
#include "../io/uart/uart.h"
#include <stdbool.h>

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

		if (c == 1 && buffer_length % 2 == 0) {
			expected_checksum = buffer[buffer_length - 2];
			buffer[buffer_length - 2] = 0;
			buffer[buffer_length - 1] = 0;

			for (int i = 2; i < buffer_length; i += 2) {
				buffer[i / 2] = buffer[i];
				buffer[i] = 0;
			}

			buffer_length -= 2;
			buffer_length /= 2;

			printf("\n%d %d %d\n", buffer_length, expected_checksum, calc_checksum());
		}
	}
}
