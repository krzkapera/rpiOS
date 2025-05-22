#include "io/gpio/gpio.h"
#include "io/system_timer/system_timer.h"
#include "io/uart/printf.h"
#include "irq/irq.h"
#include "power/power.h"
#include "sd/fatfs/ff.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

extern uint32_t get_el();
extern unsigned char _end;

void main() {
	uart_init();
	puts("EL: ");
	putchar(get_el() + '0');
	printf("\nCompilation time: " __DATE__ " " __TIME__ "\n");

	irq_init_vectors();
	enable_core0_interrupt_controller();
	irq_enable();

	// long xd = PERIPHERAL_BASE + 0x2000d0;
	// mmio_write(xd, mmio_read(xd) | 0b10);

	// FATFS fs;
	// FRESULT fr;
	// fr = f_mount(&fs, "", 1);
	// if (fr != FR_OK) {
	// 	printf("f_mount() failed, fr = %d\n", fr);
	// 	while (1)
	// 		;
	// }

	// FIL file;
	// char buffer[256];
	// UINT bytesRead;

	// fr = f_open(&file, "issue.txt", FA_READ);
	// if (fr != FR_OK) {
	// 	printf("Nie można otworzyć issue.txt: %d\n", fr);
	// 	return;
	// }

	// // 3. Czytanie pliku w pętli
	// printf("Zawartość issue.txt:\n");
	// do {
	// 	fr = f_read(&file, buffer, sizeof(buffer) - 1, &bytesRead);
	// 	if (fr != FR_OK) {
	// 		printf("Błąd odczytu: %d\n", fr);
	// 		break;
	// 	}

	// 	buffer[bytesRead] = 0; // zakończ tekst znakiem null
	// 	printf("%s", buffer);
	// } while (bytesRead > 0);

	// // 4. Zamknięcie pliku
	// f_close(&file);

	// puts(buffer);
	// puts("\n");

	// UINT bytesWritten;
	// fr = f_open(&file, "tstfilx.txt", FA_WRITE | FA_CREATE_ALWAYS);
	// if (fr != FR_OK) {
	// 	printf("Nie można otworzyć test_file.txt do zapisu: %d\n", fr);
	// 	while (1)
	// 		;
	// }

	// // 3. Dane do zapisania
	// const char* text = "To jest tekstscik do pliku.\n";

	// // 4. Zapis danych
	// fr = f_write(&file, text, strlen(text), &bytesWritten);
	// if (fr != FR_OK || bytesWritten != strlen(text)) {
	// 	printf("Błąd zapisu: %d\n", fr);
	// } else {
	// 	printf("Zapisano %u bajtów do test_file.txt\n", bytesWritten);
	// }

	// // 5. Zamknięcie pliku
	// f_close(&file);

	gpio_function(42, GPIO_FUNCTION_OUTPUT);

	for (int i = 0; i < 5; i++) {
		// mmio_write(GPSET1, 1 << 10);
		puts("1\n");
		// wait(2e6);
		// mmio_write(GPCLR1, 1 << 10);
		puts("2\n");
		// wait(2e6);
	}

	while (1) {
	}
}
