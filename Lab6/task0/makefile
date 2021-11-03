all: mypipe

mypipe: mypipe.o
	gcc -m32 -g -Wall -o mypipe mypipe.o 

mypipe.o: mypipe.c
	gcc -g -Wall -m32  -c -o mypipe.o mypipe.c

.PHONY: clean

clean: 
	rm -f *.o mypipe
