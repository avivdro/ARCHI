all: bs hexaPrint

bs: bubblesort.c
	gcc -g -m32 -Wall -o bs bubblesort.c

hexaPrint: hexaPrint.c
	gcc -g -m32 -Wall -o hexaPrint hexaPrint.c

.PHONY: clean

clean:
	rm -f * .o bs
