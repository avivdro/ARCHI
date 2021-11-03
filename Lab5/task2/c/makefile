#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: myShell

# Tool invocations
# Executable "hello" depends on the files numbers.o and main.o and add.s.
myShell: myshell.o LineParser.o looper.o
	gcc -g -m32 -Wall -o myShell myshell.o LineParser.o

myshell.o: myshell.c
	gcc -g -m32 -Wall -c -o myshell.o myshell.c

LineParser.o: LineParser.c
	gcc -g -m32 -Wall -c -o LineParser.o LineParser.c

looper.o: looper.c
	gcc -g -m32 -Wall -c -o looper.o looper.c

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean:
	rm -f *.o myShell
