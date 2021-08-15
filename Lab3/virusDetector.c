#include <stdio.h>
#include <stdlib.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;


//Task 1=========================================
virus* readVirus(FILE* fp){
	virus *v = (virus*)malloc(sizeof(virus));
	if (fread(&(v->SigSize), 2, 1, fp) != 1){
		return NULL;
	}
	fread(v->virusName, 16, 1, fp);

	v->sig = (unsigned char*)malloc(v->SigSize);
	fread(v->sig, v->SigSize, 1, fp);
	return v;
}

void printVirus(virus *vir, FILE *fp)
{
    int i;

    fprintf(fp, "\nVirus name: %s\n", vir->virusName);
    fprintf(fp, "Virus size: %hu\n", vir->SigSize);
    fprintf(fp, "Signature:\n");

    for (i = 0; i < vir->SigSize; i++)
        fprintf(fp, "%02X ", vir->sig[i]);

    fprintf(fp, "\n");
}



//===========================================
int main(int argc, char** argv){
	printf("Hello\n");

	FILE* fp = fopen("signatures-L", "rb");
	unsigned char header[4];
	fread(header, 4, sizeof(char), fp);

	virus* v = readVirus(fp);
	while (v!=NULL){
		printVirus(v, stdout);
		v = readVirus(fp);
	}

	return 0;
}
