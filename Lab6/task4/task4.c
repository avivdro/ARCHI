#include "LineParser.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#define STDIN 0
#define STDOUT 1

//Global variables
typedef struct vardecl{
  char* name;
  char* value;
  struct vardecl* next_var;
}vardecl;

int debug;
vardecl* GlobalEnviroment;

#define BUFFER_SIZE 2048
vardecl* list_append(vardecl* , char* , char* );
void list_print(vardecl *, FILE* );
void list_free(vardecl *);
char *find(vardecl*,char *const);
void cdHome();
void pipeCommands(cmdLine*);

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
/*execute custom command*/
int exeCustomeCommand(cmdLine* command){
  int isCustome=0;
  if(strcmp(command->arguments[0],"quit")==0){
    isCustome=1;
    quit(EXIT_SUCCESS);
    }
  if(strcmp(command->arguments[0],"cd")==0){
    isCustome=1;
    if(strcmp(command->arguments[1],"~")==0){
        cdHome();
    }
    else if(chdir(command->arguments[1])<0){
      perror("Could not execute CD command");
    }
  }
  if(strcmp(command->arguments[0],"set")==0){
    isCustome=1;
    GlobalEnviroment=list_append(GlobalEnviroment,command->arguments[1],command->arguments[2]);
  }
  if(strcmp(command->arguments[0],"vars")==0){
    isCustome=1;
    list_print(GlobalEnviroment,stdout);
  }
  return isCustome;
}
/*execute command*/
void execute(cmdLine* pCmdLine){
  int input=STDIN;
  int ouput=STDOUT;
  if(!exeCustomeCommand(pCmdLine)){
    int cpid;
    if(!(cpid=fork())){
      /*redirection handling*/
        if(pCmdLine->inputRedirect){
        close(input);
        input=open(pCmdLine-> inputRedirect, O_RDONLY);
        if(input<0){
            int error_code = errno;
            perror(strerror(error_code));
            exit(EXIT_FAILURE);
        }
      }
        if(pCmdLine->outputRedirect){
        close(ouput);
        ouput=open(pCmdLine-> outputRedirect, O_WRONLY | O_CREAT);
        if(ouput<0){
            int error_code = errno;
            perror(strerror(error_code));
            exit(EXIT_FAILURE);
        }
      }
        if(pCmdLine->next!=NULL){
            pipeCommands(pCmdLine);
        }
        else if(execvp(pCmdLine->arguments[0],pCmdLine->arguments)<0){
            int error_code = errno;
            perror(strerror(error_code));
            quit(EXIT_FAILURE);
      }
    }
    if(debug){print_debug(cpid,pCmdLine->arguments[0]);}
    if(pCmdLine->blocking){
      waitpid(cpid,NULL,0);
    }
  }
}
/*make new varDecl to add to the enviroment*/
vardecl* addVarDecl(char* name, char* value){
    vardecl* new_vardecl = calloc(1,sizeof(struct vardecl));
    new_vardecl->name=calloc(1,strlen(name));
    new_vardecl->value=calloc(1,strlen(value));
    strcpy(new_vardecl->name,name);
    strcpy(new_vardecl->value,value);
    return new_vardecl;
}
/*checks if the var's name already taken and replave its value if is*/
int checkAndSet(vardecl* _vardecl,char* name, char* value){
  if (strcmp(_vardecl->name,name)==0)
  {
    free(_vardecl->value);
    strcpy(_vardecl->value,value);
    return 1; 
  }
  return 0;
}
/* Add a new link with the given data to the list
   and return a pointer to the list (i.e., the first link in the list).
   If the list is null - create a new entry and return a pointer to the entry.*/
vardecl* list_append(vardecl* enviroment, char* name, char* value){
    if(enviroment==NULL){
        vardecl* new_node = addVarDecl(name,value);
        enviroment=new_node;
    }
    else{
          //checks if the name already assign to a value;
        if(checkAndSet(enviroment,name,value)){return enviroment;}
        enviroment->next_var=list_append(enviroment->next_var,name,value);
    }
    return enviroment;
}
/* Free the memory allocated by the list. */
void list_free(vardecl *enviroment){
    vardecl* curr_node=enviroment;
    if(curr_node!=NULL){
        list_free(curr_node->next_var);
        free(curr_node->name);
        free(curr_node->value);
        free(curr_node);
    }
    return;
}
/*print single var decleration*/
void printValDecl(vardecl* _vardecl, FILE* output){
    fprintf(output,"name:%s\t", _vardecl->name);
    fprintf(output,"value:%s\n", _vardecl->value);
}
/* Print the data of every link in list to the given stream. Each item followed by a newline character. */
void list_print(vardecl *enviroment, FILE* stream){
    vardecl* curr_link= enviroment;
    while(curr_link!=NULL){
        printValDecl(curr_link,stream);
        curr_link=curr_link->next_var;
    }
}
/*replace all vars to their values*/
void replace(vardecl* enviroment,cmdLine *pCmdLine){
    for(int i=0;i<pCmdLine->argCount;i++){
        if (strncmp(pCmdLine->arguments[i],"$",1)==0){
            char* value = find(enviroment,pCmdLine->arguments[i]+1);
            if (value==NULL){
                perror("There is not variable with this name\n");
            }
            else{
                replaceCmdArg(pCmdLine,i,value);
            }
        }
    }
}
/*return var values*/
char *find(vardecl* enviroment,char *const name) {
    vardecl* env = enviroment;
    while (env!=NULL){
        if (strcmp(env->name,name)==0){return env->value;}
        env=env->next_var;
    }
    return NULL;
}
/*rolling to home directory*/
void cdHome(){
    char* dest = getenv("HOME");
    while(strcmp(dest,getcwd(NULL,PATH_MAX))!=0){
        if(chdir("..")<0)
            perror("Could not find the Home directory\n");
    }
}
/*handling single piped command*/
void pipeCommands(cmdLine* input_command){
    pid_t child1_pid , child2_pid;
    int fileDescriptors [2]; //file descriptor for 2 childs
    if (pipe(fileDescriptors)==-1){
        int error_code = errno;
        perror(strerror(error_code));
        exit(EXIT_FAILURE);
    }
    /*creating the first child process*/
    child1_pid = fork();
    if (child1_pid==-1){
        int error_code = errno;
        perror(strerror(error_code));
        //exit(EXIT_FAILURE); - ask Peter
    }
    /*the first child will be enter*/
    if (!child1_pid){
        close(STDOUT);
        dup2(fileDescriptors[1],STDOUT);
        close(fileDescriptors[1]);

        execvp(input_command->arguments[0] ,input_command->arguments);
        int error_code = errno;
        perror(strerror(error_code));
        _exit(EXIT_FAILURE);
    }

        /*creating the second child process*/
    else {
        close(fileDescriptors[1]); // closing the write -fileDescriptor
        //forking child 2
        child2_pid = fork();
        if (child2_pid==-1){
            int error_code = errno;
            perror(strerror(error_code));
            //exit(EXIT_FAILURE); - ask Peter
        }
        /*the second child will be enter*/
        if (!child2_pid)
        {
            close(STDIN);
            dup2(fileDescriptors[0],STDIN);
            close(fileDescriptors[0]);
            execvp(input_command->next->arguments[0] ,input_command->next->arguments);
            int error_code = errno;
            perror(strerror(error_code));
            _exit(EXIT_FAILURE);
        }
        else {
            close(fileDescriptors[0]);
            waitpid (child1_pid , NULL , 0);
            waitpid (child2_pid , NULL , 0);
        }
    }
}
/*item at utility for list*/
vardecl* ItemAt(vardecl* node, int index){
    if(index==0){
        return node;
    }
    else{
        return ItemAt(node->next_var,index-1);
    }
}
int main(int argc, char const *argv[]) {

  FILE* input = stdin;
  GlobalEnviroment=NULL;

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
    replace(GlobalEnviroment,line);
    execute(line);
    freeCmdLines(line);
  }
  return 0;
}
