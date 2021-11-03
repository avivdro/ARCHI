#include "LineParser.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#define BUFFER_SIZE 2048

void quit(int exit_code){
  exit(exit_code);
}

void printCWD(){
  char path_name[PATH_MAX];
  getcwd(path_name,PATH_MAX);
  fprintf(stdout, "%s\n",path_name);
}

void execute(cmdLine* pCmdLine){
  if(strcmp(pCmdLine->arguments[0],"quit")==0){quit(EXIT_SUCCESS);}
  int return_val=execvp(pCmdLine->arguments[0],pCmdLine->arguments);
  if(return_val<0){
    perror("Could not execute the command");
    quit(EXIT_FAILURE);
  }
}

int main(int argc, char const *argv[]) {

  FILE* input = stdin;
  char* buffer[2048];
  int active=1;

  while(1){
    printCWD();
    fgets(buffer,BUFFER_SIZE,input);
    cmdLine* line = parseCmdLines(buffer);
    execute(line);
    freeCmdLines(line);
  }
  return 0;
}
