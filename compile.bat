@echo off

C:\Users\nicol\AppData\Local\bin\NASM\nasm.exe -f elf32 srcs\entry\gdt.asm -o outputs\gdt.o
wsl as --32 srcs/entry/kernel_entry.asm -o outputs/kernel_entry.o

:: compile C/C++ resources
wsl gcc -w -ffreestanding -m32 -c srcs/stdio/stdio.c -o outputs/stdio.o -Iincludes/
wsl gcc -w -ffreestanding -m32 -c srcs/utils/ports.c -o outputs/ports.o -Iincludes/
wsl gcc -w -ffreestanding -m32 -c srcs/utils/util.c -o outputs/util.o -Iincludes/
wsl gcc -w -ffreestanding -m32 -c srcs/entry/init.cpp -o outputs/init.o -Iincludes/ -I. -fno-threadsafe-statics -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -fno-pie

wsl ld -melf_i386 -T srcs/linker.ld outputs/* -o outputs/temp.elf

wsl mkdir outputs/tmp
wsl mkdir outputs/tmp/boot
wsl mkdir outputs/tmp/boot/grub
wsl mv outputs/temp.elf outputs/tmp/boot/kernel.bin

wsl cp srcs/grub.cfg outputs/tmp/boot/grub/grub.cfg

wsl grub-mkrescue --output=result/bootable.iso outputs/tmp
wsl rm -rf outputs/*