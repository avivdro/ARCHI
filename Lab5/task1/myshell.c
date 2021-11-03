#include "LineParser.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//Global variables
int debug;

#define BUFFER_SIZE 2048

void quit(int exit_code){
  _exit(exit_code);
}

void printCWD(){
  char path_name[PATH_MAX];
  getcwd(path_name,PATH_MAX);
  fprintf(stdout, "%s\n",path_name);
}

void print_debug(int cpid,char* command){
  fprintf(stderr, "Child PID: %d command: %s\n",cpid,command );
}

int exeCustomeCommand(cmdLine* command){
  int isCustome=0;
  if(strcmp(command->arguments[0],"quit")==0){
    isCustome=1;
    quit(EXIT_SUCCESS);
    }
  if(strcmp(command->arguments[0],"cd")==0){
    isCustome=1;
    if(chdir(command->arguments[1])<0){
      perror("Could not execute CD command");
    }
  }
  return isCustome;
}

void execute(cmdLine* pCmdLine){
  if(!exeCustomeCommand(pCmdLine)){
    int cpid;
    if(!(cpid=fork())){
        if(execvp(pCmdLine->arguments[0],pCmdLine->arguments)<0){
          perror("Could not execute command");
          quit(EXIT_FAILURE);
      }
    }
    if(debug){print_debug(cpid,pCmdLine->arguments[0]);}
    if(pCmdLine->blocking){
      waitpid(cpid,NULL,0);
    }
  }
}

int main(int argc, char const *argv[]) {

  FILE* input = stdin;
  char buffer[2048];
  int active=1;

  /*parsing arguments*/
  debug=0;
  for(int i=1;i<argc;i++){
    if( (strcmp("-D",argv[i])==0)){
      debug=1;
    }
  }

  while(active){
    printCWD();
    fgets(buffer,BUFFER_SIZE,input);
    cmdLine* line = parseCmdLines(buffer);
    execute(line);
    freeCmdLines(line);
  }
  return 0;
}
