all: encoder

encoder: encoder.o util.o start.s
	ld -m elf_i386 start.o encoder.o util.o -o encoder

start.o: start.s
	nasm -f elf32 start.s -o start.o

util.o: util.c util.h
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

encoder.o: encoder.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector encoder.c -o encoder.o

.PHONY: clean

clean:
	rm -f * .o encoder
	