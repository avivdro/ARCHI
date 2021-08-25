#include "util.h"


#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19
#define SEEK_SET 0

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDRW 2
#define O_CREATE 64

#define STDIN 0
#define STDOUT 1
#define STDERR 2


extern int system_call(int sysCall, int arg1, int arg2, int arg3);
/* this removes the error??*/

char filter(char c){
  /*param c: character*/
  /*return: lowercase of c, if it was Uppercase*/
  if ('A' <= c && c <= 'Z')
    c = c+32;
  return c;
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


/* MAIN ================================================= */
int main (int argc , char* argv[], char* envp[]){
  char c;
  int active = 1;
  int callReturn;
  int valid_argument = 1;
  int debug = 0;
  int input = STDIN;
  int output = STDOUT;
  int i;


  /* FLAGS ======================================== */
  for(i = 1; i < argc; i++){
    valid_argument = 0;
    if(strcmp("-D",argv[i])== 0){
      valid_argument = 1;
      debug = 1;
      system_call(SYS_WRITE, STDOUT, "Debug mode ON\n", 14);
    }
    if(strncmp("-o",argv[i],2*sizeof(char)) == 0){
      valid_argument = 1;
      int fileDescOutputFile;
      fileDescOutputFile = system_call(SYS_OPEN, argv[i]+2*sizeof(char),O_WRONLY | O_CREATE, 0777);
      if(fileDescOutputFile < 0) { errorHandler(STDOUT,"Cannot open output file.\n");}
      else{output=fileDescOutputFile;}
      if (debug){
        system_call(SYS_WRITE, STDOUT, "Output file is: ", 16);
        system_call(SYS_WRITE, STDOUT, argv[i]+2*sizeof(char), strlen(argv[i]+2*sizeof(char)));
        system_call(SYS_WRITE, STDOUT, "\n", 1);
      }
    }
    
    if(strncmp("-i",argv[i],2*sizeof(char)) == 0){
      valid_argument = 1;
      int fileDescInputFile;
      fileDescInputFile = system_call(SYS_OPEN,argv[i]+2*sizeof(char),O_RDONLY,0777);
      if(fileDescInputFile < 0) { errorHandler(STDOUT,"Cannot open input file.\n");}
      else{input = fileDescInputFile;}
      if (debug){
        system_call(SYS_WRITE, STDOUT, "Input file is: ", 15);
        system_call(SYS_WRITE, STDOUT, argv[i]+2*sizeof(char), strlen(argv[i]+2*sizeof(char)));
        system_call(SYS_WRITE, STDOUT, "\n", 1);
      }

  }
    if(!valid_argument){
      errorHandler(output,"Invalid argument given\n");
    }
  }

  if (input == STDIN){system_call(SYS_WRITE, STDOUT, "Input file is: STDIN\n", 21);}
  if (output==STDOUT){system_call(SYS_WRITE, STDOUT, "Output file is: STDOUT\n", 23);}


  /*MAIN PROGRAM ==========================================================================*/
  while (active){
    callReturn = system_call(SYS_READ, input, &c, 1);  /* reading single char from STDIN */
    /*if(debug){printDebug(SYS_READ, callReturn);}*/

    if (c == '\n') {active = 0;}
    else{
      c = filter(c);
      callReturn = system_call(SYS_WRITE, output, &c, 1);

      /*if(debug){printDebug(SYS_WRITE, callReturn);}*/
    }
  }

  system_call(SYS_WRITE, STDOUT, "Done!\n" , 6);
  system_call(SYS_CLOSE,input,0,0);
  return 0;
}
