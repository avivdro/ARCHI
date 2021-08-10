#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct fun_desc {
  char *name;
  char (*fun)(char);
};

char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}

char encrypt(char c){
  if (0x20<=c && c<=0x7F)
    c = c+3;
  return c;
}

char decrypt(char c){
  if (0x20<=c && c<=0x7F)
    c = c-3;
  return c;
}

char cprt(char c){
  if (0x20<=c && c<=0x7F)
    printf("%c", c);
  else
    printf(".");
  return c;
}

char my_get(char c){
  int in = fgetc(stdin);
  return in;
}


char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  for (int i=0; i<array_length; i++){
    mapped_array[i] = f(array[i]);
  }
  return mapped_array;
}

int print_menu(struct fun_desc menu[]){
  //my added function
  //gets the menu list, returns number of options, and prints the menu
  int cont2 = 1;
  int i = 0;
  while (cont2){
    printf("%d: %s\n", i, menu[i].name);
    i++;
    if (menu[i].name == NULL) cont2 = 0;
  }
  return i;
}

int main(int argc, char **argv){
  struct fun_desc menu[] = {{"Censor", censor}, {"Encrypt", encrypt}, {"Decrypt", decrypt}, {"Print string", cprt}, {"Get string", my_get}, {NULL, NULL}};

  int base_len = 5;
  char* carray = (char*)(malloc(base_len*sizeof(char)));


  char input = 0;
  int i = print_menu(menu);
  while (1){

    input = fgetc(stdin);  //get input
    
    if (!((48<=input && input<=47+i)||(input==10))){
      //check input is valid range. 48 is '0' ascii
      //printf("DEBUG - input is %d", input);
      printf("Not within bounds. Exiting.\n");
      exit(1);
    }
    
    else{
      if(input != 10){
        fgetc(stdin); //clean the buffer
        printf("Within bounds.\n");
        carray = map(carray, base_len, menu[input-48].fun);
        printf("\n");
      }
    }

    if (input != 10) print_menu(menu); //print the menu list
  }
}
