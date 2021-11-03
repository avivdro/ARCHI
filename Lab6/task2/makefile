#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: myShell

# Tool invocations
# Executable "hello" depends on the files numbers.o and main.o and add.s.
myShell: task2.o LineParser.o
	gcc -g -m32 -Wall -o myShell task2.o LineParser.o

task2.o: task2.c
	gcc -g -m32 -Wall -c -o task2.o task2.c

LineParser.o: LineParser.c
	gcc -g -m32 -Wall -c -o LineParser.o LineParser.c

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean:
	rm -f *.o myShell
