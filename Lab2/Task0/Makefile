all: count-words

count-words: count-words.c
	gcc -g -m32 -Wall -o count-words count-words.c

.PHONY: clean

clean:
	rm -f * .o count-words
