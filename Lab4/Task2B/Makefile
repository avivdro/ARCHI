all: flame

flame: flame.o util.o start.s
	ld -m elf_i386 start.o flame.o util.o -o flame

start.o: start.s
	nasm -f elf32 start.s -o start.o

util.o: util.c util.h
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

flame.o: flame.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector flame.c -o flame.o

.PHONY: clean

clean:
	rm -f * .o flame
	