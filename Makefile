SRC_DIR = src
BUILD_DIR = build

CFILES = $(shell find $(SRC_DIR) -type f -name "*.c")
OFILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(CFILES))
#  -nostdinc
GCCFLAGS = -Wall -O0 -ffreestanding -nostdlib -nostartfiles
GCCPATH = /opt/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf/bin

all: clean $(BUILD_DIR)/output/kernel8.img

$(BUILD_DIR)/boot.o: $(SRC_DIR)/boot.S | $(BUILD_DIR)
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# $(BUILD_DIR)/entry.o: $(SRC_DIR)/irq/entry.S | $(BUILD_DIR)
# 	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# $(BUILD_DIR)/irq_S.o: $(SRC_DIR)/irq/irq.S | $(BUILD_DIR)
# 	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

$(BUILD_DIR)/output/kernel8.img: $(BUILD_DIR)/boot.o $(OFILES) # $(BUILD_DIR)/entry.o $(BUILD_DIR)/irq_S.o
	$(GCCPATH)/aarch64-none-elf-ld -nostdlib $^ -T $(SRC_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
	$(GCCPATH)/aarch64-none-elf-objcopy -O binary $(BUILD_DIR)/kernel8.elf $(BUILD_DIR)/output/kernel8.img
	cp $(BUILD_DIR)/output/kernel8.img /mnt/j/kernel8.img
	sudo umount /mnt/j
	powershell.exe -Command "(New-Object -comObject Shell.Application).Namespace(17).ParseName('Q:').InvokeVerb('Eject'); Start-Sleep -Seconds 1"
	@date +"%b %d %Y %H:%M:%S"

$(BUILD_DIR):
	@$(shell mkdir -p $(dir $(OFILES)))
	@sudo mount -t drvfs Q: /mnt/j 2> /dev/null || (echo "Nie ma karty" && exit 1)
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/output

clean:
	@/bin/rm -rf $(BUILD_DIR) > /dev/null 2> /dev/null || true
