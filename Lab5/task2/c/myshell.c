#include "LineParser.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 2048

/*Values for process status*/
#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process{
    cmdLine* cmd;           /* parsed command line*/
    pid_t pid; 		        /* id of process running the command*/
    int status;             /* process status: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	/* next process of chain*/
} process;

/*Global variables*/
int debug;
process* Globalprocess_list;

/*Fucntion Declaration*/
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
void delete_single_process(process* );
int deleteTerminatedProcesses(process** );

/*coverting status int to status string*/
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

/*EXECUTE THE COMMANDS*/
int exeCustomCommand(cmdLine* command){
    int isCustom=0;
    if(strcmp(command->arguments[0],"quit")==0){
        isCustom=1;
        freeCmdLines(command);
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
    else if(strcmp(command->arguments[0],"kill")==0){
        isCustom=1;
        int pid = atoi(command->arguments[1]);
        if(kill(pid,SIGINT)==-1){
            int error_code = errno;
            perror(strerror(error_code));
        }
        else
        {printf("%d handling SIGINT\n",pid);}
    }
    else if(strcmp(command->arguments[0],"suspend")==0){
        isCustom=1;
        int pid = atoi(command->arguments[1]);
        if(kill(pid,SIGTSTP)==-1){
            int error_code = errno;
            perror(strerror(error_code));
        }
        else
        {printf("%d handling SIGTSTP\n",pid);}
    }
    else if(strcmp(command->arguments[0],"wake")==0){
        isCustom=1;
        int pid = atoi(command->arguments[1]);
        if(kill(pid,SIGCONT)==-1){
            int error_code = errno;
            perror(strerror(error_code));
        }
        else
        {printf("%d handling SIGCONT\n",pid);}
    }
    if(isCustom){freeCmdLines(command);}
    return isCustom;
}

/* Add a new link with the given data to the end of the list
   and return a pointer to the list (i.e., the first link in the list).
   If the list is null - create a new entry and return a pointer to the entry.*/
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
}

/* Free the memory allocated by the list. */
void free_list(process* process_list){
    process* curr_process=process_list;
    if(curr_process!=NULL){
        free_list(curr_process->next);
        delete_single_process(curr_process);

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
    //(*process_list)=deleteTerminatedProcesses((*process_list));
    while(deleteTerminatedProcesses(process_list)){};
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
        int wait = waitpid(curr_process->pid,&status,WNOHANG | WUNTRACED | WCONTINUED);
        printf("pid: %d status: %x, wait: %d\n",curr_process->pid,status,wait);
        if(wait!=0)
        {
            updateProcessStatus(curr_process,curr_process->pid,status);
        }
        curr_process=curr_process->next;
    }
}

/*update specific process status*/
void updateProcessStatus(process* process_list, int pid, int status){
    int w_status=RUNNING;
    if(WIFEXITED(status) || WIFSIGNALED(status)){w_status=TERMINATED;}
    else if(WIFSTOPPED(status)){w_status=SUSPENDED;}
    else if(WIFCONTINUED(status)){w_status=RUNNING;}
    process_list->status=w_status;
    printf("Updating PID %d status to %s\n",process_list->pid,getStatusString(w_status));
}

/*handling free a single process node*/
void delete_single_process(process* todelete){
    freeCmdLines(todelete->cmd);
    todelete->cmd=NULL;
    todelete->next=NULL;
    free(todelete);
    todelete=NULL;
}

/*delete the first occurrence of a process with TERMINATED status*/
int deleteTerminatedProcesses(process** process_list){
    process* curr_process = *process_list;
    process* prev_process;
    /*handling deleting the head if its terminated*/
    if(curr_process!=NULL && curr_process->status==TERMINATED){
        *process_list=curr_process->next;
        delete_single_process(curr_process);
        return 1;
    }
    /*iterate over the list and stop in the next terminated process*/
    while (curr_process!=NULL && curr_process->status!=TERMINATED)
    {
        prev_process=curr_process;
        curr_process=curr_process->next;
    }
    /*if got to the end of the list and we did not delete anything*/
    if(curr_process==NULL)
    {
        return 0;
    }

    else{
        /*not NULL and TERMINATED, then deleting the node and signal that deleting had been occurde*/
        prev_process->next=curr_process->next;
        delete_single_process(curr_process);
        return 1;
    }
}

/*MAIN ==================================================================================*/
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
