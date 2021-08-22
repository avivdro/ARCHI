#include "util.h"


#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19
#define SYS_GETDENTS 141 
#define SEEK_SET 0

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDRW 2
#define O_CREATE 64
#define O_RWX 0777

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define BUFFER_SIZE 8192  /*given by instructions*/

/* ============================================================== */

extern int system_call(int sysCall, int arg1, int arg2, int arg3);
/* this removes the error??*/

/* Struct for linux files */
typedef struct linux_dirent {
   unsigned long  ino;        /* Inode number */
   unsigned long  offset;     /* Offset to next linux_dirent */
   unsigned short len;        /* Length of this linux_dirent */
   char           name[];     /* Filename (null-terminated) */
}linux_dirent;

void errorHandler(int output,char* errorMessage){
  system_call(SYS_WRITE,output,errorMessage, strlen(errorMessage));
  system_call(SYS_EXIT,0x55,0,0);
}

void printDebug(int sys_call, int return_val){
  char tab = '\t';
  char newline = '\n';
  system_call(SYS_WRITE, STDERR, "DEBUG: System call: ", 20);
  system_call(SYS_WRITE,STDERR,itoa(sys_call),strlen(itoa(sys_call)));
  system_call(SYS_WRITE,STDERR,&tab,1);
  system_call(SYS_WRITE, STDERR, " - return value: ", 17);
  system_call(SYS_WRITE,STDERR,itoa(return_val),strlen(itoa(return_val)));
  system_call(SYS_WRITE,STDERR,&newline,1);
}

void printDebugFile(linux_dirent* file){

}


/* FUNCTION FOR PRINTING THE FILE INFO */
int printFile(int output,linux_dirent* file,int debugFlag){
  int callReturn;
  callReturn = system_call(SYS_WRITE, output, file->name, strlen(file->name));
  system_call(SYS_WRITE, output, "\n", 1);

  if(callReturn<0){errorHandler(output,"Error in writing name of file\n");}
  if(debugFlag){printDebug(SYS_WRITE, callReturn);printDebugFile(file);}

  return file->len;
}


/* MAIN ================================================================== */
int main (int argc , char* argv[], char* envp[]){
  system_call(SYS_WRITE, STDOUT, "Flame strikes!\n", 15);

  int output = STDOUT;
  int fd, debug, prefix, i, validArgument, dirLength;
  char buffer[BUFFER_SIZE];
  linux_dirent* file;
  debug=0;


  /* FLAGS =============================================================== */
  for(i = 1;i < argc; i++){
    validArgument = 0;
    if(strcmp("-D", argv[i]) == 0){
      validArgument=1;
      debug=1;
    }

    if (!validArgument){
      errorHandler(output,"Invalid argument given\n");
    }
  }

  /*MAIN PROGRAM =============================================================*/
  
  fd = system_call(SYS_OPEN,".",O_RDONLY,O_RWX);  /*Get file descriptor*/
  if(debug) {printDebug(SYS_OPEN,fd);}
  if(fd<=0) {errorHandler(output,"Error in opening directory\n");}

  dirLength = system_call(SYS_GETDENTS,fd,buffer,BUFFER_SIZE); /*get buffer of the struct of the files in the directory*/
  if(debug){printDebug(SYS_GETDENTS,dirLength);}
  if(dirLength<0){errorHandler(output,"Error in getdents system call\n");}

  i=0;
  while(i < dirLength){
    file=(linux_dirent*)(buffer+i); /*Get the next file*/
    if(strcmp(file->name,".")!=0 && strcmp(file->name,"..")!=0){ /* discarding prints of the root and the prev directory file descriptor*/
      printFile(output,file,debug);
    }
    i=i+file->len;
  }

  
  /*END =======================================================================*/
  system_call(SYS_WRITE, STDOUT, "Done!\n" , 6);
  system_call(SYS_CLOSE, STDIN,0,0);
  return 0;
}
