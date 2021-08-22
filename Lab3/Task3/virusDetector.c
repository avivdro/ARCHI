#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

typedef struct link link;
 
struct link {
    link *nextVirus;
    virus *vir;
};

char menu[] = "Menu:\n1) Load signatures.\n2) Print signatures.\n3) Detect viruses.\n4) Fix viruses.\n";


//Task 1a=========================================
virus* readVirus(FILE* fp, int big_endian){
	virus *v = (virus*)malloc(sizeof(virus));
	if (fread(&(v->SigSize), 2, 1, fp) != 1){
		return NULL;
	}
	if (big_endian){
		int swapped = (v->SigSize>>8) | (v->SigSize<<8);
		v->SigSize = swapped;
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

//Task 1b=============================================
void list_print(link *virus_list, FILE* stream){
	printf("VIRUSES:\n");
    link* curr_link= virus_list;
    while(curr_link!=NULL){
        printVirus(curr_link->vir,stream);
        curr_link=curr_link->nextVirus;
    }
}
link* list_append(link* virus_list, virus* data)
{
    link *new_link = (link *)malloc(sizeof(link));
    new_link->nextVirus = virus_list;
    new_link->vir = data;
    return new_link;
}
void list_free(link *virus_list)
{
    virus *v;
    link *temp;
    while (virus_list != NULL) 
    {
        v = virus_list->vir;
        free(v->sig);
        free(v);
        temp = virus_list->nextVirus;
        free(virus_list);
        virus_list = temp;
    }
}
link* loadVirusListFromFile(link *virus_list){
	char* filename=NULL;
    FILE* file;
    printf("Input the signature file name:\n");
    scanf("%ms",&filename);
    file = fopen(filename,"rb");
    if(file==NULL){ // if file can't be opened
        fprintf(stderr,"Error in opening file: %s\n",filename);
   		free(filename);
        return virus_list;
    }

    unsigned char header[4];
	fread(header, 4, sizeof(char), file);
	int big_endian = 0;  //0 is little, 1 is big
	if (header[3] == 'B') big_endian = 1;

	virus* v;
	v = readVirus(file, big_endian);
	while (v!=NULL){
		//printf("Reading virus- %s\n", v->virusName);
		virus_list = list_append(virus_list, v);
		v = readVirus(file, big_endian);
	}
	fclose(file);
	return virus_list;
}

//Task1c====================================
link* virusNumberI(link* node, int index){
    if(index==0)
        return node;
    return virusNumberI(node->nextVirus,index-1);
}

int get_list_size(link* node){
    if(node==NULL)
        return 0;
    return get_list_size(node->nextVirus)+1;
}
int compare_virus(char* buffer, unsigned short rest_of_buffer, virus* virus){
    int result=-1;
    if(rest_of_buffer>=virus->SigSize){
        result = memcmp(buffer,virus->sig,virus->SigSize);
    }
    return result;
}


void detect_virus(char *buffer, unsigned int size, link *virus_list, FILE* output){
	int count = 0;
	for(int offset=0;offset<size;offset++){
        for(int i=0;i<get_list_size(virus_list);i++){
            virus* virus = NULL;
            virus = virusNumberI(virus_list,i)->vir;
            if(compare_virus(buffer+offset,size-offset,virus)==0){
            	count++;
                printf("Starting byte: %d\n",offset);
                printf("Virus name: %s\n", virus->virusName);
                printf("Virus size: %d\n\n", virus->SigSize);
            }
        }
    }
    if (count==0){printf("No viruses were found.\n");}
}

unsigned int readVirusFile(char* filename, char* buffer){
	FILE* file;
	file = fopen(filename,"rb");
	if(file==NULL){ // if file can't be opened
    	return 0;
    }
    unsigned int len = fread(buffer, 1, 10240, file);
    rewind(file);
    fclose(file);
    return len;
}

//Task2b =============================================================

void kill_virus(char *fileName, int signitureOffset, int signitureSize){
    FILE *fp = fopen(fileName, "r+");
    fseek(fp, signitureOffset, SEEK_SET);
    for (int i = 0; i < signitureSize; i++)
        fputc(0x90, fp);
    fclose(fp);
    printf("VIRUS KILLED\n");
}

void handle_kill(char* filename){
	int offset, size;
	printf("Input starting offset:\n");
	scanf("%d", &offset);
	printf("Input size of virus:\n");
	scanf("%d", &size);
	printf("Going to kill at %d for %d bytes\n", offset, size);
	kill_virus(filename, offset, size);
}

//===========================================
int main(int argc, char** argv){
	printf("Hello!\n");

	char input ='0';
	//virus* v;
	printf("%s", menu);
	scanf(" %c",&input);
	link* list = (link *)malloc(sizeof(link));
	list = NULL;
	while (1){
		if (input=='1'){ //Load signatures
			list = NULL;
			//fgetc(stdin);
			list = loadVirusListFromFile(list);
		}
		if (input=='2'){ // Print signatures
			if (list==NULL){printf("Virus list is empty.\n");}
			else {list_print(list, stdout);}
		}
		if (input=='3'){ // Detect viruses
			//printf("option 3\n");
			//fgetc(stdin);

			char *buffer = (char*)malloc((10240)*sizeof(char*));
			unsigned int fileLength= readVirusFile(argv[1], buffer);
			if (fileLength==0){
				printf("Invalid file given.\n");
			}
			else{
			    detect_virus(buffer, fileLength, list, stdout);
			}
		}
		if (input == '4'){ //Fix the viruses
			handle_kill(argv[1]);
		}

		if (input<'1' || '4' < input){
			printf("Goodbye!\n");
			break;
		}

		// FINALLY:
		printf("%s", menu);
		scanf(" %c",&input);
	}
	list_free(list);

	return 0;
}
