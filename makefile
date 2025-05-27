CC = ~/opt/cross/bin/i386-elf-gcc
OBJCOPY = ~/opt/cross/bin/i386-elf-objcopy
OFLAGS = -O binary
CFLAGS = -ffreestanding -O2 -Wall -Wextra -nostdlib -fno-strict-aliasing -mno-80387 -lgcc -c
USERCFLAGS = -Wl,--no-warn-rwx-segments -ffreestanding -nostdlib -lgcc -I user/lib -T user/apps.ld ct/ackylib.o
LDFLAGS = -T linker.ld -o acky.bin -ffreestanding -O2 -nostdlib -lgcc
AS = nasm
ASFLAGS = -f bin
OBJS =	ct/boot.o ct/kmain.o ct/vga.o ct/io.o ct/memory.o ct/gdt.o ct/idt.o ct/ps2kbd.o ct/herr.o ct/pit.o ct/ata.o\
		ct/serial.o ct/fs.o ct/process.o ct/pitasm.o ct/faultHandlers.o ct/acpi.o

# Enabling KVM improves the accuracy of the emulation
QEMUCMD = qemu-system-i386 -enable-kvm -drive file=acky.iso,format=raw,media=disk -m 124 -serial stdio -display gtk,zoom-to-fit=on -cpu host

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
	@$(CC) $(CFLAGS) foki/acpi.c -o ct/acpi.o

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
	@cp ct/*.aef iso/progdat/

	@echo "Hai! Test" > iso/test.txt
	@grub-mkrescue -o acky.iso iso

userspace:
	@echo "Building userspace..."
	@$(AS) -f elf32 user/lib/acky.asm -o ct/ackylib.o

	@$(AS) $(ASFLAGS) user/system/kproc.asm -o ct/kproc.asa
	@$(AS) $(ASFLAGS) user/system/init.asm -o ct/init.asa

	@$(AS) $(ASFLAGS) user/apps/test2.asm -o ct/test2.aef
	# @$(AS) $(ASFLAGS) user/apps/shell.asm -o ct/shell.aef

	@$(CC) $(USERCFLAGS) -DAEF_NAME="\"Shell\"" user/apps.c user/apps/shell.c -o ct/shell.elf
	@$(OBJCOPY) $(OFLAGS) ct/shell.elf ct/shell.aef

	@$(CC) $(USERCFLAGS) -DAEF_NAME="\"Process Info\"" user/apps.c user/apps/procinfo.c -o ct/procinfo.elf
	@$(OBJCOPY) $(OFLAGS) ct/procinfo.elf ct/pinfo.aef

	@$(CC) $(USERCFLAGS) -DAEF_NAME="\"Starter\"" user/apps.c user/apps/start.c -o ct/start.elf
	@$(OBJCOPY) $(OFLAGS) ct/start.elf ct/start.aef

	@$(CC) $(USERCFLAGS) -DAEF_NAME="\"CTest\"" user/apps.c user/apps/ctest.c -o ct/ctest.elf
	@$(OBJCOPY) $(OFLAGS) ct/ctest.elf ct/ctest.aef

	@$(CC) $(USERCFLAGS) -DAEF_NAME="\"Power state\"" user/apps.c user/apps/pstate.c -o ct/pstate.elf
	@$(OBJCOPY) $(OFLAGS) ct/pstate.elf ct/pstat.aef

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