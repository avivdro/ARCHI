#include <stdio.h>
#include <stdlib.h>


void printHex(unsigned char* c, int len){
	for (int i = 0; c[i]!='\0'; i++){
		printf("%02x ", c[i]);
	}
	printf("\n");
}


int main(int argc, char **argv) {
	unsigned char buffer[100];

	if (argc==1){
		printf("No file name was given, exiting.\n");
		exit(1);
	}

	FILE *f = fopen(argv[1], "r");

	if (f==NULL){
		printf("No such file exists, exiting.\n");
		exit(2);
	}

	fread(buffer, sizeof(buffer), 1, f);
	printHex(buffer, 100);

	fclose(f);
	return 0;
}