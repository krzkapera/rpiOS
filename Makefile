SRC_DIR = src
BUILD_DIR = build

CFILES = $(shell find $(SRC_DIR) -type f -name "*.c")
SFILES = $(shell find $(SRC_DIR) -type f -name "*.S")
OFILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(CFILES)) \
         $(patsubst $(SRC_DIR)/%.S,$(BUILD_DIR)/%_S.o,$(SFILES))

GCCFLAGS = -Wall -O0 -ffreestanding -nostdlib -nostartfiles -mgeneral-regs-only
GCCPATH = /opt/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf/bin

.PHONY: sd pi all

all: $(BUILD_DIR)/output/kernel8.img | /tmp/mypipe
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -c $(SRC_DIR)/kernel.c -o $(BUILD_DIR)/kernel.o

$(BUILD_DIR)/%_S.o: $(SRC_DIR)/%.S | $(BUILD_DIR)
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -D__ASM__ -c $< -o $@
	$(GCCPATH)/aarch64-none-elf-size $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@
	$(GCCPATH)/aarch64-none-elf-size $@

$(BUILD_DIR)/output/kernel8.img: $(OFILES)
	$(GCCPATH)/aarch64-none-elf-ld -nostdlib $^ -T $(SRC_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
	$(GCCPATH)/aarch64-none-elf-objcopy -O binary $(BUILD_DIR)/kernel8.elf $(BUILD_DIR)/output/kernel8.img

sd: all
	@sudo mount -t drvfs Q: /mnt/j 2> /dev/null || (echo "Nie ma karty" && exit 1)
	cp $(BUILD_DIR)/output/kernel8.img /mnt/j/kernel8.img || exit 1
	sudo umount /mnt/j
	powershell.exe -Command "(New-Object -comObject Shell.Application).Namespace(17).ParseName('Q:').InvokeVerb('Eject'); Start-Sleep -Seconds 1"
	# ../plink.exe -serial COM7 -sercfg 115200,8,n,1,N

pi: all
	python3 update.py

/tmp/mypipe:
	mkfifo /tmp/mypipe

$(BUILD_DIR):
	@$(shell mkdir -p $(dir $(OFILES)))
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/output

clean:
	@/bin/rm -rf $(BUILD_DIR) > /dev/null 2> /dev/null || true
