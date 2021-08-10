#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int addr5;
int addr6;

int foo();
void point_at(void *p);
void foo1();
void foo2();

int main (int argc, char** argv){
    int addr2;
    int addr3; 
    char* yos="ree";
    int * addr4 = (int*)(malloc(50));
    printf("- &addr2: %p\n",&addr2);  //stack  
    printf("- &addr3: %p\n",&addr3);  //stack
    printf("- foo: %p\n",foo);        //code
    printf("- &addr5: %p\n",&addr5);  //data
    
	point_at(&addr5);
	
    printf("- &addr6: %p\n",&addr6);  //data
    printf("- yos: %p\n",yos);        //data
    printf("- addr4: %p\n",addr4);    //data
    printf("- &addr4: %p\n",&addr4);  //stack
    
    printf("- &foo1: %p\n" ,&foo1);   //code
    printf("- &foo1: %p\n" ,&foo2);   //code
    printf("- &foo2 - &foo1: %ld\n" ,&foo2 - &foo1);

    //SIZEOF LONG = 8 BYTES

    //Task1C:
    printf("Task1C -------------------------------------------------\n");
    int iarray[3];
    printf("iarray - %p, %p, %p\n", &iarray[0], &iarray[1], &iarray[2]);
    float farray[3];
    printf("farray - %p, %p, %p\n", &farray[0], &farray[1], &farray[2]);
    double darray[3];
    printf("darray - %p, %p, %p\n", &darray[0], &darray[1], &darray[2]);
    char carray[3];
    printf("carray - %p, %p, %p\n", &carray[0], &carray[1], &carray[2]);
    int* iptrarray[3];
    char* cptrarray[3];
    printf("Values:\n");
    printf("iarray-    %p\n", iarray);
    printf("iarray+1 - %p\n", iarray+1);
    printf("farray-    %p\n", farray);
    printf("farray+1 - %p\n", farray+1);
    printf("darray-    %p\n", darray);
    printf("darray+1 - %p\n", darray+1);
    printf("carray-    %p\n", carray);
    printf("carray+1 - %p\n", carray+1);
    printf("iptrarray-    %p\n", iptrarray);
    printf("iptrarray+1 - %p\n", iptrarray+1);
    printf("cptrarray-    %p\n", cptrarray);
    printf("cptrarray+1 - %p\n", cptrarray+1);

    printf("Task1D -------------------------------------------------\n");
    int iarray2[] = {1,2,3};
    char carray2[] = {'a','b','c'};
    int* iarrayPtr;
    char* carrayPtr;

    iarrayPtr = &iarray2[0];
    carrayPtr = &carray2[0];
    for (int i=0; i<3; i++){
        printf("iarray[%d]: %d,   carray[%d]: %c\n", i, *iarrayPtr, i, *carrayPtr);
        iarrayPtr++;
        carrayPtr++;
    }
    int* pInt;
    printf("Uninitialized pointer: %p\n", pInt);


    return 0;
}

int foo(){
    return -1;
}

void point_at(void *p){
    int local;
	static int addr0 = 2;
    static int addr1;


    long dist1 = (size_t)&addr6 - (size_t)p;
    long dist2 = (size_t)&local - (size_t)p;
    long dist3 = (size_t)&foo - (size_t)p;
    
    printf("dist1: (size_t)&addr6 - (size_t)p: %ld\n",dist1);
    printf("dist2: (size_t)&local - (size_t)p: %ld\n",dist2);
    printf("dist3: (size_t)&foo - (size_t)p:  %ld\n",dist3);
	
	printf("- addr0: %p\n", & addr0);
    printf("- addr1: %p\n",&addr1);
}

void foo1 (){    
    printf("foo1\n"); 
}

void foo2 (){    
    printf("foo2\n");    
}
