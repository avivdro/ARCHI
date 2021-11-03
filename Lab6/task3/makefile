#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: mypipeline

# Tool invocations
# Executable "hello" depends on the files numbers.o and main.o and add.s.
mypipeline: task3.o
	gcc -g -m32 -Wall -o mypipeline task3.o

task3.o: task3.c
	gcc -g -m32 -Wall -c -o task3.o task3.c

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean:
	rm -f *.o mypipeline
