CC = ~/opt/cross/bin/i386-elf-gcc
CFLAGS = -ffreestanding -O2 -Wall -Wextra -nostdlib -mno-80387 -lgcc -c
LDFLAGS = -T linker.ld -o fmsos.bin -ffreestanding -O2 -nostdlib -lgcc
AS = nasm
ASFLAGS = -felf32

QEMUCMD = qemu-system-i386 -drive file=fmsos.iso,format=raw,media=disk -m 124 -serial stdio

all: build mkiso clean

build:
	mkdir ct

	$(AS) $(ASFLAGS) multiboot.asm -o ct/boot.o
	$(AS) $(ASFLAGS) fmsk/gdt.asm -o ct/gdt.o

	$(CC) $(CFLAGS) fmsk/kmain.c -o ct/kmain.o
	$(CC) $(CFLAGS) fmsk/vga.c -o ct/vga.o
	$(CC) $(CFLAGS) fmsk/io.c -o ct/io.o
	$(CC) $(CFLAGS) fmsk/memory.c -o ct/memory.o
	$(CC) $(CFLAGS) fmsk/idt.c -o ct/idt.o
	$(CC) $(CFLAGS) fmsk/ps2kbd.c -o ct/ps2kbd.o
	$(CC) $(CFLAGS) fmsk/herr.c -o ct/herr.o
	$(CC) $(CFLAGS) fmsk/pit.c -o ct/pit.o
	$(CC) $(CFLAGS) fmsk/ata.c -o ct/ata.o
	$(CC) $(CFLAGS) fmsk/serial.c -o ct/serial.o
	$(CC) $(CFLAGS) fmsk/fs.c -o ct/fs.o

	$(CC) $(LDFLAGS) ct/boot.o ct/kmain.o ct/vga.o ct/io.o ct/memory.o ct/gdt.o ct/idt.o ct/ps2kbd.o ct/herr.o ct/pit.o ct/ata.o ct/serial.o ct/fs.o
	stat fmsos.bin

mkiso:
	mkdir -p iso/boot/grub/
	cp grub.cfg iso/boot/grub/grub.cfg
	cp fmsos.bin iso/boot/

	grub-mkrescue -o fmsos.iso iso

clean:
	rm -r ct/
	rm fmsos.bin
	rm -r iso/

run:
	$(QEMUCMD)

debug:
	bochs -q -dbg

love:
	echo "o///o"