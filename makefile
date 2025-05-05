CC = ~/opt/cross/bin/i386-elf-gcc
CFLAGS = -ffreestanding -O2 -Wall -Wextra -nostdlib -mno-80387 -lgcc -c
LDFLAGS = -T linker.ld -o acky.bin -ffreestanding -O2 -nostdlib -lgcc
AS = nasm
ASFLAGS = -f bin
OBJS = ct/boot.o ct/kmain.o ct/vga.o ct/io.o ct/memory.o ct/gdt.o ct/idt.o ct/ps2kbd.o ct/herr.o ct/pit.o ct/ata.o ct/serial.o ct/fs.o ct/process.o ct/pitasm.o ct/faultHandlers.o

QEMUCMD = qemu-system-i386 -drive file=acky.iso,format=raw,media=disk -m 124 -serial stdio -display gtk,zoom-to-fit=on

all: build userspace mkiso clean

build:
	@echo "Building kernel..."
	@mkdir ct

	@$(AS) -felf32 multiboot.asm -o ct/boot.o
	@$(AS) -felf32 foki/pit.asm -o ct/pitasm.o

	@$(CC) $(CFLAGS) foki/kmain.c -o ct/kmain.o
	@$(CC) $(CFLAGS) foki/gdt.c -o ct/gdt.o
	@$(CC) $(CFLAGS) foki/vga.c -o ct/vga.o
	@$(CC) $(CFLAGS) foki/io.c -o ct/io.o
	@$(CC) $(CFLAGS) foki/memory.c -o ct/memory.o
	@$(CC) $(CFLAGS) foki/idt.c -o ct/idt.o
	@$(CC) $(CFLAGS) foki/ps2kbd.c -o ct/ps2kbd.o
	@$(CC) $(CFLAGS) foki/herr.c -o ct/herr.o
	@$(CC) $(CFLAGS) foki/pit.c -o ct/pit.o
	@$(CC) $(CFLAGS) foki/ata.c -o ct/ata.o
	@$(CC) $(CFLAGS) foki/serial.c -o ct/serial.o
	@$(CC) $(CFLAGS) foki/fs.c -o ct/fs.o
	@$(CC) $(CFLAGS) foki/process.c -o ct/process.o
	@$(CC) $(CFLAGS) foki/faultHandlers.c -o ct/faultHandlers.o

	@$(CC) $(LDFLAGS) $(OBJS)
	@stat acky.bin

mkiso:
	@echo "Building ISO..."
	@mkdir -p iso/boot/grub/
	@cp grub.cfg iso/boot/grub/grub.cfg

	@mkdir iso/ackydat
	@cp acky.bin iso/ackydat/
	@cp ct/*.asa iso/ackydat/

	@mkdir iso/progdat
	@cp ct/*.bin iso/progdat/

	@echo "Hai! Test" > iso/test.txt
	@grub-mkrescue -o acky.iso iso

userspace:
	@echo "Building userspace..."
	@$(AS) $(ASFLAGS) user/system/kproc.asm -o ct/kproc.asa
	@$(AS) $(ASFLAGS) user/system/init.asm -o ct/init.asa

	@$(AS) $(ASFLAGS) user/apps/test2.asm -o ct/test2.bin
	@$(AS) $(ASFLAGS) user/apps/test3.asm -o ct/test3.bin
	@$(AS) $(ASFLAGS) user/apps/shell.asm -o ct/shell.bin
	@$(CC) -nostdlib -nostartfiles -ffreestanding -Ttext 0 -e main -o ct/ctest.elf user/apps/ctest.c
	@objcopy -O binary ct/ctest.elf ct/ctest.bin
	ndisasm -b 32 ct/ctest.bin

clean:
	@echo "Cleaning up..."
	@rm -r ct/
	@rm acky.bin
	@rm -r iso/

run:
	$(QEMUCMD)

debug:
	@bochs -q -dbg

love:
	@echo "o///o"