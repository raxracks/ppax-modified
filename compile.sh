rm -rf outputs/tmp
rm -rf outputs/tmp/boot
rm -rf outputs/tmp/boot/grub

as --32 srcs/entry/kernel_entry.asm -o outputs/kernel_entry.o

#compile C/C++ resources
gcc -w -ffreestanding -fno-stack-protector -m32 -c srcs/stdio/stdio.c -o outputs/stdio.o -Iincludes/
gcc -w -ffreestanding -fno-stack-protector -m32 -c srcs/utils/ports.c -o outputs/ports.o -Iincludes/
gcc -w -ffreestanding -fno-stack-protector -m32 -c srcs/utils/util.c -o outputs/util.o -Iincludes/
gcc -w -ffreestanding -fno-stack-protector -m32 -c srcs/input/keyboard.c -o outputs/keyboard.o -Iincludes/
gcc -w -ffreestanding -fno-stack-protector -m32 -c srcs/graphics/vbe.c -o outputs/vbe.o -Iincludes/
gcc -w -ffreestanding -fno-stack-protector -m32 -c srcs/interpreter/absorba.c -o outputs/absorba.o -Iincludes/
gcc -w -ffreestanding -fno-stack-protector -m32 -c srcs/entry/init.c -o outputs/init.o -Iincludes/ -I. -fno-threadsafe-statics -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -fno-pie

ld -melf_i386 -T srcs/linker.ld outputs/* -o outputs/temp.elf

mkdir outputs/tmp
mkdir outputs/tmp/boot
mkdir outputs/tmp/boot/grub
mv outputs/temp.elf outputs/tmp/boot/kernel.bin

cp srcs/grub.cfg outputs/tmp/boot/grub/grub.cfg

grub-mkrescue --output=result/bootable.iso outputs/tmp
rm -rf outputs/*
qemu-system-x86_64 -cdrom result/bootable.iso -m 1G $1 $2
