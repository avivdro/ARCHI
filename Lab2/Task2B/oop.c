#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Task 2B------------------------------------------------------------------
char encrypt(char c);
/* Gets a char c and returns its encrypted form by adding 3 to its value. 
If c is not between 0x20 and 0x7E it is returned unchanged */

char decrypt(char c);
/* Gets a char c and returns its decrypted form by reducing 3 to its value. 
If c is not between 0x20 and 0x7E it is returned unchanged */

char cprt(char c);
/* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed 
by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns 
the value of c unchanged. */

char my_get(char c);
/* Ignores c, reads and returns a character from stdin using fgetc. */


//Task 2A -------------------------------------------------------------------
char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}

char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  
  /* TODO: Complete during task 2.a */
  for (int i=0; i<array_length; i++){
    mapped_array[i] = f(array[i]);
  }
  
  return mapped_array;
}


int main(int argc, char **argv){

  /* TODO: Test your code */

  char arr1a[] = {'H','E','Y','!'};
  char* arr2a = map(arr1a, 4, censor);

  printf("Input: %s\n", arr1a);
  printf("Output: %s\n", arr2a); 
  printf("Done with T2A.\n");
  free(arr2a);

  printf("Task2B----------------------------------------\n");
  int base_len = 5;
  char arr1[base_len];
  char* arr2 = map(arr1, base_len, my_get);
  char* arr3 = map(arr2, base_len, encrypt);
  char* arr4 = map(arr3, base_len, cprt);
  char* arr5 = map(arr3, base_len, decrypt);
  char* arr6 = map(arr5, base_len, cprt);
  free(arr2);
  free(arr3);
  free(arr4);
  free(arr5);
  free(arr6);
}


//Task 2B Implementation---------------------------------------

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
    fputc(c, stdout);
  else
    fputc('.', stdout);
  fputc('\n', stdout);
  return c;
}

char my_get(char c){
  c = fgetc(stdin);
  return c;
}
