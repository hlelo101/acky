# Acky OS
**Acky OS is an operating system made for the x86 architecture.**\
Its kernel (named Foki) supports:
- Multitasking
> A simple scheduler with a FIFO queue (with an exception being processes in a "waiting" state) and context switching to make all of that work
- Process isolation
> Processes run in ring-3, in a memory isolated environment
- Simple drivers
> This includes drivers for the following devices (mostly standard x86 stuff):
> - ATA drive
> - VGA Text mode
> - PS2 Keyboard
> - PIT
> - Serial output
- System calls
> You can use system calls to start a program, print a character, get a text input from the user, ...
- The ISO9660 filesystem
> You can navigate through the folders to load some files.
# The filesystem
Acky's system files are mixed with GRUB's, so you will see some files/directories useful to GRUB and Acky in the root of the disk.\
Acky's directories are:
- ackydat
> This directory holds Acky's system files.
- progdat
> This directory holds program files.
# The shell
The shell is pretty basic. You can start an app with it.\
To do so, simply enter the *full* path of the application you want to run.\
For example, to run test2.bin, enter `A:/PROGDAT/TEST2.BIN`.

It is planned to make a more complex shell in the future.
# What is the goal of this project?
Acky isn't meant in any ways to become mainstream or a popular OS.\
The goal of this project is mainly to recreate the OS of my dreams: I used to think a lot (since I was 12 I think?) about a fictional general-purpose operating system who's UI is kind of like the WII's, but also like Windows', with a taskbar, start menu (called "Start center" as it does much more than the start menu). Its look is kind of a modernized version of the 2000s asthetics. It also supports pretty complex stuff such as dynamic kernel swapping, for example.\
And now I'm trying to recreate that, to make it real.\
Yeah, it's pretty ambitious, and I'm probably never going to implement things like dynamic kernel swapping, but I'll try to go as far as I can.