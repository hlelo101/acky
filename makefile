CC = ~/opt/cross/bin/i386-elf-gcc
CFLAGS = -ffreestanding -O2 -Wall -Wextra -nostdlib -mno-80387 -lgcc -c
LDFLAGS = -T linker.ld -o fmsos.bin -ffreestanding -O2 -nostdlib -lgcc
AS = nasm
ASFLAGS = -f bin
OBJS = ct/boot.o ct/kmain.o ct/vga.o ct/io.o ct/memory.o ct/gdt.o ct/idt.o ct/ps2kbd.o ct/herr.o ct/pit.o ct/ata.o ct/serial.o ct/fs.o ct/process.o ct/pitasm.o ct/faultHandlers.o

QEMUCMD = qemu-system-i386 -drive file=fmsos.iso,format=raw,media=disk -m 124 -serial stdio -display gtk,zoom-to-fit=on

all: build userspace mkiso clean

build:
	mkdir ct

	$(AS) -felf32 multiboot.asm -o ct/boot.o
	$(AS) -felf32 fmsk/pit.asm -o ct/pitasm.o

	$(CC) $(CFLAGS) fmsk/kmain.c -o ct/kmain.o
	$(CC) $(CFLAGS) fmsk/gdt.c -o ct/gdt.o
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
	$(CC) $(CFLAGS) fmsk/process.c -o ct/process.o
	$(CC) $(CFLAGS) fmsk/faultHandlers.c -o ct/faultHandlers.o

	$(CC) $(LDFLAGS) $(OBJS)
	stat fmsos.bin

mkiso:
	mkdir -p iso/boot/grub/
	cp grub.cfg iso/boot/grub/grub.cfg
	cp fmsos.bin iso/boot/

	mkdir iso/ackysys
	cp ct/*.bin iso/ackysys/

	echo "Hai! Test" > iso/test.txt
	grub-mkrescue -o fmsos.iso iso

userspace:
	$(AS) $(ASFLAGS) apps/kproc.asm -o ct/kproc.bin

	$(AS) $(ASFLAGS) apps/init.asm -o ct/init.bin
	$(AS) $(ASFLAGS) apps/test2.asm -o ct/test2.bin
	$(AS) $(ASFLAGS) apps/test3.asm -o ct/test3.bin

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