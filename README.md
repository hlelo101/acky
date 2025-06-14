# Acky OS
**Acky OS is an operating system made to run on x86-based computers.**\
Its kernel (named Foki) supports:
- Preemptive Multitasking
> A simple scheduler with a FIFO queue (with an exception being processes in a "waiting" state) and context switching to make all of that work\
> Inter-Process Communication
- Process isolation
> Processes run in ring-3, in a memory isolated environment
- Simple drivers
> This includes drivers for the following devices (mostly standard x86 stuff):
> - ATA drive
> - VBE
> - PS2 Keyboard
> - PIT
> - Serial output
- System calls
> You can use system calls to start a program, print a character, get a text input from the user, ...
- The ISO9660 filesystem
> You can navigate through the folders to load some files.
- Shutting down with ACPI
> Can go through ACPI tables, find and "parse" _S5, etc.
# The filesystem
Acky's system files are mixed with GRUB's, so you will see some files/directories useful to GRUB and Acky in the root of the disk.\
Acky's directories are:
- ackydat
> This directory holds Acky's system files.
- progdat
> This directory holds program files.
# The shell
The shell is pretty basic. You can start an app with it.\
To do so, simply enter the name or full path of the application you want to run.\
For example, to run test2.bin, you can enter `TEST2.AEF` or `A:/PROGDAT/TEST2.AEF`.

It is planned to make a more complex shell in the future.
# The GUI
Right now, the GUI is very experimental and buggy. You might see things like artifacts.\
You can move the mouse around and click on some buttons.
# The AEF file format
AEF stands for **A**cky **E**xecutable **F**ile.\
It's simply a custom header for applications on Acky.\
The header is made out of 3 fields:
| Name | Size/Type | Description |
| :--- | :-------: | :---------- |
| Program name | 32 bytes | The name of the program |
| Entry point | i32 integer | The entry point of the program/its start |
| Signature | 3 bytes | Used to identify AEF files. Should always be "AEF". |
> You can see how this is implemented in the [startub stub](https://github.com/hlelo101/fms-os/blob/main/user/apps.c) and the [apps](https://github.com/hlelo101/fms-os/tree/main/user/apps) written in assembly.
# Building
You will need to have a i386 version of GCC in `~/opt/cross`. The OSDev wiki has a great tutorial on [how to do that](https://wiki.osdev.org/GCC_Cross-Compiler).\
Then, simply run `make`. Here are the available targets:
```bash
make        # Build Acky
make clean  # Clean up
make run    # Run with QEMU
make debug  # Run with Bochs with a debugger
```
# What is the goal of this project?
The goal of this project is mainly to recreate the OS of my dreams: I used to think a lot (since I was 12 I think?) about a fictional general-purpose operating system who's UI is kind of like the WII's, but also like Windows', with a taskbar, start menu (called "Start center" as it does much more than the start menu). Its look is kind of a modernized version of the 2000s asthetics. It also supports pretty complex stuff such as dynamic kernel swapping, for example.\
And now I'm trying to recreate that, to make it real.\
Yeah, it's pretty ambitious, and I'm probably never going to implement things like dynamic kernel swapping, but I'll try to go as far as I can.
