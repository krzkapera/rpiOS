CFILES = $(wildcard *.c)
OFILES = $(CFILES:.c=.o)
#  -nostdinc
GCCFLAGS = -Wall -O0 -ffreestanding -nostdlib -nostartfiles
GCCPATH = /opt/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf/bin

all: clean kernel8.img

boot.o: boot.S
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -c boot.S -o boot.o

%.o: %.c
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

kernel8.img: boot.o $(OFILES)
	$(GCCPATH)/aarch64-none-elf-ld -nostdlib boot.o $(OFILES) -T link.ld -o kernel8.elf
	$(GCCPATH)/aarch64-none-elf-objcopy -O binary kernel8.elf kernel8.img
	sudo mount -t drvfs J: /mnt/j
	cp kernel8.img /mnt/j/kernel8.img
	sudo umount /mnt/j
	powershell.exe -Command "(New-Object -comObject Shell.Application).Namespace(17).ParseName('J:').InvokeVerb('Eject'); Start-Sleep -Seconds 2"


clean:
	/bin/rm kernel8.elf *.o *.img > /dev/null 2> /dev/null || true
