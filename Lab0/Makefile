all: lab0

lab0: main.c numbers.o add.o
	gcc -g -m32 -Wall -o lab0 main.c numbers.o add.o 

add.o: add.s
	nasm -f elf -o add.o add.s

numbers.o: numbers.c
	gcc -g -m32 -Wall -c -o numbers.o numbers.c

main.o: main.c
	gcc -g -m32 -c -o main.o main.c

.PHONY: clean

clean:
	rm -f *.o lab0

