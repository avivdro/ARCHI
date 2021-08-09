all: lab1

lab1: main.c
	gcc -g -m32 -Wall -o lab1 main.c

main.o: main.c
	gcc -g -m32 -c -o main.o main.c

.PHONY: clean

clean:
	rm -f *.o lab1

