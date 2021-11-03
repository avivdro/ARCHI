#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#define STDIN 0
#define STDOUT 1

void print_debug (int pid , char* message){
  if (pid != -1)
      fprintf(stderr,"%s:%d\n",message,pid);
  else
      fprintf(stderr,"%s\n",message);
}


int main(int argc, char** argv) {
    int debug=0;
    pid_t child1_pid , child2_pid;

    /*parsing command line arguments*/
    for (int i=0; i< argc; i++){
        if (strncmp(argv[i],"-d",2)==0||strncmp(argv[i],"-D",2) == 0)
            debug =1;
    }
    char* const ls[3] = {"ls","-l",0};
    char* const tail[4] = {"tail","-n","2",0};
    int fileDescriptors [2]; //file descriptor for 2 childs
    if (pipe(fileDescriptors)==-1){
      int error_code = errno;
      perror(strerror(error_code));
      exit(EXIT_FAILURE);
    }
    /*creating the first child process*/
    if (debug){print_debug(-1 , "parent_process>forking…");}
    child1_pid = fork();
    if (child1_pid==-1){
        int error_code = errno;
        perror(strerror(error_code));
        //exit(EXIT_FAILURE); - ask Peter
    }
    if(debug){print_debug(getpid() , "parent_process>created process with id");}
    /*the first child will be enter*/
    if (!child1_pid){
        if(debug){print_debug(-1,"child1>redirecting stdout to the write end of the pipe…");}
        close(STDOUT);
        dup2(fileDescriptors[1],STDOUT);
        close(fileDescriptors[1]);

        if(debug){print_debug (-1 , "child1>going to execute cmd:…");}
        execvp(ls[0] , ls);
        int error_code = errno;
        perror(strerror(error_code));
        _exit(EXIT_FAILURE);
    }

    /*creating the second child process*/
    else {
        if(debug){print_debug(-1 ,"parent_process>closing the write end of the pipe…");}
        close(fileDescriptors[1]); // closing the write -fileDescriptor
        //forking child 2
        if(debug){print_debug(-1,"parent_process>forking…");}
        child2_pid = fork();
        if (child2_pid==-1){
            int error_code = errno;
            perror(strerror(error_code));
            //exit(EXIT_FAILURE); - ask Peter
            }
        if (debug){print_debug(getpid(),"parent_process>created process with id");}
        /*the second child will be enter*/
        if (!child2_pid)
        {
            if (debug) {print_debug (-1 , "child2>redirecting stdin to the read end of the pipe…");}
            close(STDIN);
            dup2(fileDescriptors[0],STDIN);
            close(fileDescriptors[0]);
            execvp(tail[0] , tail);
            int error_code = errno;
            perror(strerror(error_code));
            _exit(EXIT_FAILURE);
         }
        else {
          if (debug) {print_debug (-1 , "parent_process>closing the read end of the pipe");}
          close(fileDescriptors[0]);

          if (debug) {print_debug (child1_pid , "parent_process>waiting for child processes to terminate PID:");}
          waitpid (child1_pid , NULL , 0);

          if (debug) {print_debug (child2_pid , "parent_process>waiting for child processes to terminate PID:");}
          waitpid (child2_pid , NULL , 0);
        }
      }
      if (debug) {
          print_debug (-1 , "parent_process>closing the read end of the pipe");
          print_debug(-1,"parent_process>exiting…");
      }
      return 0;
}
