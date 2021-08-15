#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus 
{
    unsigned short sigSize;
    char virusName[16];
    unsigned char *sig;
} virus;

typedef struct link link;
 
struct link {
    link *nextVirus;
    virus *vir;
};

virus *readVirus(FILE *fp, int big_endian) 
{
    virus *vir = (virus *)malloc(sizeof(virus));
    fread(vir, 18, 1, fp);
    if (big_endian)
        vir->sigSize = (vir->sigSize << 8) + (vir->sigSize >> 8);
    vir->sig = (unsigned char *)malloc(vir->sigSize);
    fread(vir->sig, vir->sigSize, 1, fp);
    return vir;
}

void printVirus(virus *vir, FILE *fp)
{
    int i;

    fprintf(fp, "\nVirus name: %s\n", vir->virusName);
    fprintf(fp, "Virus size: %hu\n", vir->sigSize);
    fprintf(fp, "signature:\n");

    for (i = 0; i < vir->sigSize; i++)
        fprintf(fp, "%02X ", vir->sig[i]);

    fprintf(fp, "\n");
}

/* Print the data of every link in list to the given stream. Each item followed by a newline character. */
void list_print(link *virus_list, FILE *fp)
{
    while (virus_list != NULL) {
        printVirus(virus_list->vir, fp);
        fprintf(fp, "\n");
    }
}
 
/* Add a new link with the given data to the list 
   (either at the end or the beginning, depending on what your TA tells you),
   and return a pointer to the list (i.e., the first link in the list).
   If the list is null - create a new entry and return a pointer to the entry. */
link* list_append(link* virus_list, virus* data)
{
    link *new_link = (link *)malloc(sizeof(link));
    new_link->nextVirus = virus_list;
    new_link->vir = data;
    return new_link;
}
 
/* Free the memory allocated by the list. */
void list_free(link *virus_list)
{
    virus *vir;
    link *temp;
    while (virus_list != NULL) 
    {
        vir = virus_list->vir;
        free(vir->sig);
        free(vir);
        temp = virus_list->nextVirus;
        free(virus_list);
        virus_list = temp;
    }
}

link *readVirusFile(char *filename) 
{
    FILE *sigs = fopen(filename, "r");
    link *list = NULL;
    virus *vir;
    int virusDefs = 0;
    int big_endian = 0;

    char header[4];
    fread(header, 4, 1, sigs);
    if (header[3] == 'B')
        big_endian = 1;

    vir = readVirus(sigs, big_endian);

    while (!feof(sigs))
    {
        list = list_append(list, vir);
        virusDefs++;
        vir = readVirus(sigs, big_endian);
    }

    printf("Virus definitions loaded: %d\n", virusDefs);

    return list;
}

void printVirusList(link *list)
{
    while (list != NULL) 
    {
        printVirus(list->vir, stdout);
        list = list->nextVirus;
    }
}

void detectSingleVirus(char *buffer, unsigned int size, unsigned int offset, virus *vir, FILE *output)
{
    if (offset + vir->sigSize <= size && memcmp(buffer, vir->sig, vir->sigSize) == 0)
    {
        printf("Detected virus: %s\n", vir->virusName);
        printf("Starting location: 0x%X\n", offset);
        printf("Virus signature size: %d\n", vir->sigSize);
    }
}

void detectVirus(char *buffer, unsigned int size, link *virus_list, FILE *output)
{
    int pos;
    link *list_pos;

    for (pos = 0; pos < size; pos++)
    {
        list_pos = virus_list;
        while (list_pos != NULL) 
        {
            detectSingleVirus(buffer + pos, size, pos, list_pos->vir, output);
            list_pos = list_pos->nextVirus;
        }
    }
}

void killVirus(char *filename, int offset, int size)
{
    int i;
    FILE *fp = fopen(filename, "r+");
    fseek(fp, offset, SEEK_SET);
    for (int i = 0; i < size; i++)
        fputc(0x90, fp);
    fclose(fp);
}

int main(int ac, char**av)
{
    char input[100];
    link *virus_list;
    int option;
    unsigned char file_buffer[10240];
    unsigned int file_size;
    int pos, size;
    FILE *suspect = NULL;

    if (ac != 2) 
    {
        printf("NAME\n"
               "\tvirusDetector - detects a virus in a file from a given set of viruses\n"
               "SYNOPSIS\n"
               "\tvirusDetector FILE\n");
        return 1;
    }

    virus_list = readVirusFile("signatures-L");

    while (1) 
    {
        printf("1) Load signatures\n2) Print signatures\n3) Detect viruses\n4) Fix file\n");
        fgets(input, 100, stdin);
        sscanf(input, "%d\n", &option);

        if (option == 1)
        {
            fgets(input, 100, stdin);
            input[strlen(input) - 1] = '\0';
            printf("%s\n", input);
            virus_list = readVirusFile(input);
        }
        else if (option == 2)
        {
            printVirusList(virus_list);
        }
        else if (option == 3)
        {
            suspect = fopen(av[1], "r");
            file_size = fread(file_buffer, 1, 10240, suspect);
            detectVirus(file_buffer, file_size, virus_list, stdout);
            fclose(suspect);
        }
        else if (option == 4)
        {
            printf("Enter starting byte location: ");
            scanf("%d", &pos);
            printf("Enter signature size: ");
            scanf("%d", &size);
            killVirus(av[1], pos, size);
        }
        else
            break;
    }
    list_free(virus_list);
    return 0;

    /* TASK 1a

    if (ac != 2)
        return 1; // TODO

    FILE *sigs = fopen(av[1], "r");
    char header[4];

    fread(header, 4, 1, sigs);

    virus *vir;
    
    while (1)
    {
        vir = readVirus(sigs);
        if (feof(sigs))
            break;
        printVirus(vir, stdout);
    }

    fclose(sigs);
    */   
}