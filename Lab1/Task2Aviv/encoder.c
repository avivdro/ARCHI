#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {

	int debugMode = 0;     //debug mode
	int minus = 0;		   //minus?
	int plus = 0;          //plus?
	int diff = 0;          //difference (0-15)
	char* fileName;        //string of input file
	int fileExists = 0;    //is there a file?
	char* outputFile;      //string of output file
	int toOutputFile = 0;  //am i writing to output file?
	
	//Check which OPTIONS where given:
	for (int i=1; i<argc; i++){
		//DEBUG MODE: -D
		if(strcmp(argv[i], "-D")==0)
			debugMode = 1;
		//Encoding with minus: -e{key}
		if(argv[i][0]== '-' && argv[i][1] == 'e'){
			minus = 1;
			diff = argv[i][2];
		}
		//Encoding with plus: +e{key}
		if(argv[i][0]== '+' && argv[i][1]== 'e'){
			plus = 1;
			diff = argv[i][2];
		}
		//Input file: -iFILE
		if(argv[i][0]=='-' && argv[i][1]=='i'){
			fileName = argv[i] + 2;
			if (debugMode){printf("%s\n", fileName);}
		}
		//Output file: -oFILE
		if(argv[i][0]=='-' && argv[i][1]=='o'){
			outputFile = argv[i] + 2;
			if (debugMode){printf("%s\n", outputFile);}
			toOutputFile = 1;
		}
	}

	//Calculate difference:
	if (diff< 58 && diff > 47) { diff = diff-48;}
	if(diff <71 && diff > 64) {diff = diff-55;}
	
	//Is there an input file?
	if( access( fileName, F_OK ) == 0 ) {
 	   fileExists = 1;
	} else {
   		printf("File does not exist.\n");}

	if (fileExists){
		FILE *fp;   //Input file
		fp = fopen(fileName, "r");
		
		FILE *out;  //Output file
		out = fopen(outputFile, "w");
		
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

            if (toOutputFile){fputc(c, out);}
            else{putchar(c);}
        }
        if (toOutputFile){
        	fputc('\n', out);
        	fputc('\r', out);}
        else{
        	putchar('\n');
        	putchar('\r');}

        if (debugMode){printf("The number of conversions: %d\n", count);}
	}
	return 0;
}

