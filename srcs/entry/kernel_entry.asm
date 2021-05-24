[bytes 64]
.set MAGIC, 0x1badb002
.set FLAGS, 7
.set CHECKSUM, -(MAGIC + FLAGS)
.set MODE_TYPE, 0
.set WIDTH, 1024  /* width */
.set HEIGHT, 768  /* height */
.set DEPTH, 32    /* bits per pixel */

.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0
    .long MODE_TYPE
    .long WIDTH
    .long HEIGHT
    .long DEPTH
    .space 4 * 13

.section .text
.extern kernelMain
.extern callConstructors
.global loader

loader:
    mov $kernel_stack, %esp
    mov $kernel_stack, %ecx
    push %eax
    push %ebx
    push %ecx
    call kernelMain

_stop:
    cli
    hlt
    jmp _stop


.section .bss
.space 200*1024*1024; # 200 MiB
kernel_stack:

