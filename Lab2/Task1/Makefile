all: addresses

count-words: addresses.c
	gcc -g -m32 -Wall -o addresses addresses.c

.PHONY: clean

clean:
	rm -f * .o addresses
