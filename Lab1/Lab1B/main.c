#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {

	int debugMode = 0;
	for (int i=1; i<argc; i++){
		if(strcmp(argv[i], "-D")==0)
			debugMode = 1;
	}

	char c;
	int x, y;
	int count = 0;
	while((c = fgetc(stdin)) != EOF){
		x = c;
		if (debugMode && (c != 10)) {fprintf(stderr, "%d ", x);}
		if ((c>64) && (c<91)){
			c = c + 32;
			count += 1;
		}
		y = c;

		if (debugMode && (c != 10)){
			fprintf(stderr, "%d\n", y);
		}
		putchar(c);
	}
	putchar('\n');

	if (debugMode)
		printf("The number of conversions: %d\n", count);
	return 0;
}

