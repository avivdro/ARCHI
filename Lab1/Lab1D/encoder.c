#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// aviv drori and tamir caspi

int main(int argc, char **argv) {

	int debugMode = 0;
	int minus = 0;
	int plus = 0;
	int diff = 0;
	char* fileName;
	int fileExists = 0;

	for (int i=1; i<argc; i++){
		if(strcmp(argv[i], "-D")==0)
			debugMode = 1;
		if(argv[i][0]== '-' && argv[i][1] == 'e'){
			minus = 1;
			diff = argv[i][2];
		}
		if(argv[i][0]== '+' && argv[i][1]== 'e'){
			plus = 1;
			diff = argv[i][2];
		}
		if(argv[i][0]=='-' && argv[i][1]=='i'){
			fileName = argv[i] + 2;
			if (debugMode){printf("%s\n", fileName);}
		}
	}
	if (diff< 58 && diff > 47) { diff = diff-48;}
	if(diff <71 && diff > 64) {diff = diff-55;}
	

	if( access( fileName, F_OK ) == 0 ) {
 	   fileExists = 1;
	} else {
   		printf("File does not exist.\n");	}

	if (fileExists){
		FILE *fp;
		fp = fopen(fileName, "r");
		
		char c;
	        int x, y;
	        int count = 0;
		while((c = fgetc(fp)) != EOF){
               		if (debugMode && (c != 10)) {fprintf(stderr, "%d ", x);}
                	//No encode - make all lowercase
                	x = c;
                	if (plus == 0 && minus == 0){
                        	if ((c>64) && (c<91)){
                                	c = c + 32;
                                	count += 1;
                        	}
                	}
                	y=c;

                	// Encode by diff
                	if (plus) {c = c + diff;}
                	if (minus) {c = c - diff;}

                	if (debugMode && (c != 10)) {fprintf(stderr, "%d\n", y);}
                	putchar(c);
        	}
        	putchar('\n');
        	putchar('\r');

        	if (debugMode){printf("The number of conversions: %d\n", count);}
	}
	return 0;
}

