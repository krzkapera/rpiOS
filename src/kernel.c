#include "io/gpio/gpio.h"
#include "io/system_timer/system_timer.h"
#include "io/uart/printf.h"
#include "irq/irq.h"
#include "power/power.h"
#include "sd/fatfs/ff.h"
#include <stdarg.h>
#include <stdint.h>

extern uint32_t get_el();
extern unsigned char _end;

void main() {
	uart_init();
	uart_write_text("EL: ");
	uart_write_byte(get_el() + '0');
	printf("\r\nCompilation time: " __DATE__ " " __TIME__ "\r\n");

	irq_init_vectors();
	enable_core0_interrupt_controller_AUX();
	irq_enable();

	long xd = PERIPHERAL_BASE + 0x2000d0;
	mmio_write(xd, mmio_read(xd) | 0b10);

	FATFS fs;
	FRESULT fr;
	fr = f_mount(&fs, "", 1);
	if (fr != FR_OK) {
		printf("f_mount() failed, fr = %d\r\n", fr);
		while (1)
			;
	}

	DIR dir;
	FILINFO fno;
	fr = f_opendir(&dir, "/");
	if (fr == FR_OK) {
		for (;;) {
			fr = f_readdir(&dir, &fno);
			if (fr != FR_OK || fno.fname[0] == 0)
				break;
			if (!(fno.fattrib & AM_DIR)) {
				printf("Plik: %s  rozmiar: %lu\r\n", fno.fname, fno.fsize);
			} else {
				printf("Katalog: %s\r\n", fno.fname);
			}
		}
		f_closedir(&dir);
	}

	FIL file;
	char buffer[256];
	UINT bytesRead;

	fr = f_open(&file, "issue.txt", FA_READ);
	if (fr != FR_OK) {
		printf("Nie można otworzyć issue.txt: %d\r\n", fr);
		return;
	}

	// 3. Czytanie pliku w pętli
	printf("Zawartość issue.txt:\r\n");
	do {
		fr = f_read(&file, buffer, sizeof(buffer) - 1, &bytesRead);
		if (fr != FR_OK) {
			printf("Błąd odczytu: %d\r\n", fr);
			break;
		}

		buffer[bytesRead] = 0; // zakończ tekst znakiem null
		printf("%s", buffer);
	} while (bytesRead > 0);

	// 4. Zamknięcie pliku
	f_close(&file);

	uart_write_text(buffer);
	uart_write_text("\r\n");

	gpio_function(42, GPIO_FUNCTION_OUTPUT);

	for (int i = 0; i < 5; i++) {
		mmio_write(GPSET1, 1 << 10);
		wait(2e6);
		mmio_write(GPCLR1, 1 << 10);
		wait(2e6);
	}

	while (1) {
	}
}
