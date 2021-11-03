#include "LineParser.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 2048

/*Values for status is 'process' types*/
#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process{
    cmdLine* cmd;                         /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	                  /* next process in chain */
} process;

/*Global variables*/
int debug;
process* Globalprocess_list;

/*Fucntion Declaretion*/
void quit(int);
void printCWD();
void print_debug(pid_t,char*);
int exeCustomCommand(cmdLine*);
process* list_append(process*, cmdLine* , pid_t );
void execute(cmdLine* );
process* makeNewProcess(cmdLine* , pid_t );
void list_print(process* );
void free_list(process* );
void printProcess(int ,process* );
void addProcess(process** , cmdLine* , pid_t );
void printProcessList(process** );
void freeProcessList(process* );
void updateProcessList(process **);
void updateProcessStatus(process* , int , int );
process* delete_single_process(process* );
process* deleteTerminatedProcesses(process* );

/*coverting status integer to status string*/
char* getStatusString(int status){
    if(status == TERMINATED){return "TERMINATED";}
    else if(status == RUNNING){return "RUNNING";}
    else if(status == SUSPENDED){return "SUSPENDED";}
    return "";
}

/*quit the program*/
void quit(int exit_code){
    freeProcessList(Globalprocess_list);
    _exit(exit_code);
}

/*prints the current working directory*/
void printCWD(){
    char path_name[PATH_MAX];
    getcwd(path_name,PATH_MAX);
    fprintf(stdout, "%s\n",path_name);
}

/*print debug details*/
void print_debug(pid_t cpid,char* command){
    fprintf(stderr, "Child PID: %d command: %s\n",cpid,command );
}

/*execute custom commands*/
int exeCustomCommand(cmdLine* command){
    int isCustom=0;
    if(strcmp(command->arguments[0],"quit")==0){
        isCustom=1;
        quit(EXIT_SUCCESS);
    }
    else if(strcmp(command->arguments[0],"cd")==0){
        isCustom=1;
        if(chdir(command->arguments[1])<0){
            perror("Could not execute CD command");
        }
    }
    else if(strcmp(command->arguments[0],"procs")==0){
        isCustom=1;
        printProcessList(&Globalprocess_list);
    }
    return isCustom;
}

/* Add a new link with the given data to the end of the list
   (either at the end or the beginning, depending on what your TA tells you),
   and return a pointer to the list (i.e., the first link in the list).
   If the list is null - create a new entry and return a pointer to the entry.
reccursive function*/
process* list_append(process* _process, cmdLine* cmd, pid_t pid){
    if(_process==NULL){
        process* new_process = makeNewProcess(cmd,pid);
        _process=new_process;
    }
    else{
        _process->next=list_append(_process->next,cmd, pid);
    }
    return _process;
}

/*execute commands including 'need to fork' commands*/
void execute(cmdLine* pCmdLine){
    if(!exeCustomCommand(pCmdLine)){
        pid_t cpid;
        if(!(cpid=fork())){
            if(execvp(pCmdLine->arguments[0],pCmdLine->arguments)<0){
                perror("Could not execute command");
                _exit(EXIT_FAILURE);
            }
        }
        if(cpid!=-1){
            addProcess(&Globalprocess_list,pCmdLine,cpid);
        }
        if(debug){print_debug(cpid,pCmdLine->arguments[0]);}
        if(pCmdLine->blocking){
            waitpid(cpid,NULL,0);
        }
    }
}

/*Generate new node which is the process*/
process* makeNewProcess(cmdLine* cmd, pid_t pid){
    process* new_process = malloc(sizeof(struct process));
    new_process->cmd=cmd;
    new_process->pid=pid;
    new_process->status=RUNNING;
    new_process->next=NULL;
    return new_process;
}

/*prints the list*/
void list_print(process* process_list){
    process* curr_process= process_list;
    int index=0;
    while(curr_process!=NULL){
        printProcess(index,curr_process);
        curr_process=curr_process->next;
        index++;
    }
    //printf("END OF WHILE IN ist_print");
}

/* Free the memory allocated by the list. */
void free_list(process* process_list){
    process* curr_process=process_list;
    if(curr_process!=NULL){
        free_list(curr_process->next);
        freeCmdLines(curr_process->cmd);
        free(curr_process->cmd);
        free(curr_process);
    }
    return;
}

/*Prints the process and its details*/
void printProcess(int index, process* process){
    char command[100]="";
    if(process!=NULL && process->cmd->argCount>0)
        for(int i=0;i<process->cmd->argCount;i++){
        const char* argument = process->cmd->arguments[i];
        strcat(command,argument);
        strcat(command," ");

    }
    printf("%d) %d\t\t%s\t\t%s\n",index, process->pid, getStatusString(process->status),command);
}

/*Receive a process list (process_list), a command (cmd), and the process id (pid) of the process running the command*/
void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
    (*process_list)=list_append((*process_list),cmd, pid);
}

/*print all the process list and update each one of the processes*/
void printProcessList(process** process_list){
    updateProcessList(process_list);
    printf("   PID\t\tSTATUS\t\tCommand\n");
    list_print((*process_list));
    //printf("END OF PRINT\n");
    (*process_list)=deleteTerminatedProcesses((*process_list));
}

/* free all the processes using help function*/
void freeProcessList(process* process_list){
    free_list(process_list);
}

/*iterate all over the process list and update each process*/
void updateProcessList(process **process_list){
    process* curr_process = (*process_list);
    while(curr_process!=NULL){
        int status;
        int w_status=RUNNING;
        int wait = waitpid(curr_process->pid,&status,WNOHANG);
        printf("status: %d, wait: %d\n",status,wait);
        if(wait!=0)
        {
            if(WIFEXITED(status) || WIFSIGNALED(status)){w_status=TERMINATED;}
            else if(WIFSTOPPED(status)){w_status=SUSPENDED;}
            else if(WIFCONTINUED(status)){w_status=RUNNING;}
            //printf("wstatus: %s\n",getStatusString(w_status));
            updateProcessStatus(curr_process,curr_process->pid,w_status);
        }
        curr_process=curr_process->next;
    }
}

/*update specific process status*/
void updateProcessStatus(process* process_list, int pid, int status){
        process_list->status=status;
}

process* delete_single_process(process* todelete){
    process* next = todelete->next;
    freeCmdLines(todelete->cmd);
    todelete->cmd=NULL;
    //todelete->next=NULL;
    free(todelete);
    //todelete=NULL;
    return next;
}
process* deleteTerminatedProcesses(process* process_list){
   if(process_list==NULL){
        return process_list;
    }
    else if(process_list->status==TERMINATED){
            process_list=delete_single_process(process_list);
            return deleteTerminatedProcesses(process_list);
        }
    else{
          return deleteTerminatedProcesses(process_list->next);
        }
    }

int main(int argc, char const *argv[]) {

    FILE* input = stdin;
    char buffer[2048];
    int active=1;
    Globalprocess_list=NULL;


    /*parsing arguments*/
    debug=0;
    for(int i=1;i<argc;i++){
        if( strcmp("-d",argv[i])==0 || strcmp("-D",argv[i])==0){debug=1;}
    }
    while(active){
        printCWD();
        fgets(buffer,BUFFER_SIZE,input);
        cmdLine* line = parseCmdLines(buffer);
        execute(line);
    }
    return 0;
}
