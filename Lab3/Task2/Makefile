all: vd

vd: virusDetector.c
	gcc -g -m32 -Wall -o vd virusDetector.c

.PHONY: clean

clean:
	rm -f * .o vd
