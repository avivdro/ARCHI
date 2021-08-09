all: encoder

encoder: encoder.c
	gcc -g -m32 -Wall -o encoder encoder.c

encoder.o: encoder.c
	gcc -g -m32 -c -o encoder.o encoder.c

.PHONY: clean

clean:
	rm -f *.o encoder

