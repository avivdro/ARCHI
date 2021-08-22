all: patch

patch: patch.o util.o start.s
	ld -m elf_i386 start.o patch.o util.o -o patch

start.o: start.s
	nasm -f elf32 start.s -o start.o

util.o: util.c util.h
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

patch.o: patch.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector patch.c -o patch.o

.PHONY: clean

clean:
	rm -f * .o patch
	