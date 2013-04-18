#! /bin/bash

echo Assembling
nasm -f elf -o start.o start.asm

echo Compiling
gcc -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o main.o main.c

gcc -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o scrn.o scrn.c

gcc -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o gdt.o gdt.c

gcc -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o idt.o idt.c

echo Linking
ld -T link.ld -o kernel.bin start.o main.o scrn.o gdt.o idt.o

echo Placing kernel in floppy.img
cat floppy/stage1 floppy/stage2 floppy/pad kernel.bin > floppy.img

echo Done!