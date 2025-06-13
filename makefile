CC = ~/opt/cross/bin/i386-elf-gcc 
OBJCOPY = ~/opt/cross/bin/i386-elf-objcopy
OFLAGS = -O binary
CFLAGS = -ffreestanding -O2 -Wall -Wextra -nostdlib -fno-strict-aliasing -mno-80387 -lgcc -c
USERCFLAGS = -Wl,--no-warn-rwx-segments -ffreestanding -nostdlib -lgcc -I user/lib -fomit-frame-pointer -T user/apps.ld ct/ackylib.o ct/ackylibc.o

LDFLAGS = -T linker.ld -o foki.bin -ffreestanding -O2 -nostdlib -lgcc
AS = nasm
ASFLAGS = -f bin
OBJS =	ct/boot.o ct/kmain.o ct/vga.o ct/io.o ct/memory.o ct/gdt.o ct/idt.o ct/ps2kbd.o ct/herr.o ct/pit.o ct/ata.o\
		ct/serial.o ct/fs.o ct/process.o ct/pitasm.o ct/faultHandlers.o ct/acpi.o ct/ps2mouse.o 

# Enabling KVM improves the accuracy of the emulation
QEMUCMD = qemu-system-i386 -enable-kvm -drive file=acky.iso,format=raw,media=disk -m 124 -serial stdio -display gtk,zoom-to-fit=on -cpu host

all:
	@make acky || { \
		printf "Build \033[0;31mfailed\033[0m\n"; \
		exit 1; \
	}
	@printf "Build finished \033[0;32msuccessfully\033[0m\n"

acky: checkTemp build userspace mkiso clean

checkTemp:
	@if [ -d ct ] || [ -f foki.bin ] || [ -d iso ]; then \
		echo "Some temporary build files still exist. Please run 'make clean' and try again."; \
		exit 1; \
	fi

build:
	@echo "Building kernel..."
	@mkdir ct

	@$(AS) -f bin foki/krme.asm -o ct/krme.kfe

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
	@$(CC) $(CFLAGS) foki/ps2mouse.c -o ct/ps2mouse.o

	@$(CC) $(LDFLAGS) $(OBJS)
	@stat foki.bin

mkiso:
	@echo "Building ISO..."
	@mkdir -p iso/boot/grub/
	@cp grub.cfg iso/boot/grub/grub.cfg

	@mkdir iso/ackydat/
	@cp foki.bin iso/ackydat/
	@cp ct/*.asa iso/ackydat/
	@cp ct/krme.kfe iso/ackydat/

	@mkdir iso/progdat/
	@cp ct/*.aef iso/progdat/

	@echo "Hai! Test" > iso/test.txt
	@mkdir iso/progdat/wm/
	@cp user/apps/wm/media/* iso/progdat/wm/
	@grub-mkrescue -o acky.iso iso >/dev/null 2>&1

userspace:
	@echo "Building userspace..."
	@$(AS) -f elf32 user/lib/acky.asm -o ct/ackylib.o
	@$(CC) -Wl,--no-warn-rwx-segments -ffreestanding -nostdlib -lgcc -I user/lib -fomit-frame-pointer -T user/apps.ld -c user/lib/acky.c -o ct/ackylibc.o

	@$(AS) $(ASFLAGS) user/system/kproc.asm -o ct/kproc.asa
	@$(AS) $(ASFLAGS) user/system/init.asm -o ct/init.asa

	@$(AS) $(ASFLAGS) user/apps/test2.asm -o ct/test2.aef

	@$(CC) $(USERCFLAGS) -DAEF_NAME="\"Shell\"" user/apps.c user/apps/shell.c -o ct/shell.elf
	@$(OBJCOPY) $(OFLAGS) ct/shell.elf ct/shell.aef

	@$(CC) $(USERCFLAGS) -DAEF_NAME="\"Process Info\"" user/apps.c user/apps/procinfo.c -o ct/procinfo.elf
	@$(OBJCOPY) $(OFLAGS) ct/procinfo.elf ct/pinfo.aef

	@$(CC) $(USERCFLAGS) -DAEF_NAME="\"Starter\"" user/apps.c user/apps/start.c -o ct/start.elf
	@$(OBJCOPY) $(OFLAGS) ct/start.elf ct/start.aef

	@$(CC) $(USERCFLAGS) -DAEF_NAME="\"CTest\"" user/apps.c user/apps/ctest.c -o ct/ctest.elf
	@$(OBJCOPY) $(OFLAGS) ct/ctest.elf ct/ctest.aef

	@$(CC) $(USERCFLAGS) -DAEF_NAME="\"Power state\"" user/apps.c user/apps/pstate.c -o ct/pstate.elf
	@$(OBJCOPY) $(OFLAGS) ct/pstate.elf ct/pstate.aef

	@$(CC) $(USERCFLAGS) -DAEF_NAME="\"IPC Test\"" user/apps.c user/apps/ipctest/ipctest.c -o ct/ipctest.elf
	@$(OBJCOPY) $(OFLAGS) ct/ipctest.elf ct/ipct.aef
	@$(CC) $(USERCFLAGS) -DAEF_NAME="\"IPC Test Child\"" user/apps.c user/apps/ipctest/ipcchild.c -o ct/ipcchild.elf
	@$(OBJCOPY) $(OFLAGS) ct/ipcchild.elf ct/ipcchild.aef

	@$(CC) $(USERCFLAGS) -DAEF_NAME="\"Terminal Write\"" user/apps.c user/apps/twrite.c -o ct/twrite.elf
	@$(OBJCOPY) $(OFLAGS) ct/twrite.elf ct/twrite.aef

	@$(CC) $(USERCFLAGS) -DAEF_NAME="\"Launcher\"" user/apps.c user/apps/launcher.c -o ct/launcher.elf
	@$(OBJCOPY) $(OFLAGS) ct/launcher.elf ct/launcher.aef

	@$(CC) $(USERCFLAGS) -DAEF_NAME="\"Window Manager\"" user/apps.c user/apps/wm/wm.c user/apps/wm/img.c\
		user/apps/wm/window.c -o ct/wm.elf
	@$(OBJCOPY) $(OFLAGS) ct/wm.elf ct/wm.aef
	@$(AS) $(ASFLAGS) user/apps/wm/media/font.asm -o user/apps/wm/media/font.pbf

clean:
	@echo "Cleaning up..."
	@if [ -d ct ]; then rm -r ct/; fi
	@if [ -f foki.bin ]; then rm foki.bin; fi
	@if [ -d iso ]; then rm -r iso/; fi	

run:
	$(QEMUCMD)

debug:
	@bochs -q -dbg

love:
	@echo "o///o"
