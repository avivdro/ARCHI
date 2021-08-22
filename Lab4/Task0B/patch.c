#include "util.h"


#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19

#define SEEK_SET 0
#define STDOUT 1
#define O_RDRW 2


int main (int argc , char* argv[], char* envp[]){
  int shiraPos = 657;
  char * nameToReplace;
  char* fileName;
  int fd;
  if (argc==3){
    fileName = argv[1];
    nameToReplace = argv[2];
    fd = system_call(SYS_OPEN,fileName,O_RDRW,0777);
    if(fd < 0){
      system_call(SYS_EXIT,0x55);
    }
    system_call(SYS_LSEEK,fd,shiraPos,SEEK_SET);
    system_call(SYS_WRITE,fd,nameToReplace,strlen(nameToReplace));
    system_call(SYS_WRITE,STDOUT,"Shira's name replaced succesfully\n",strlen("Shira's name replaced succesfully\n"));
    system_call(SYS_CLOSE,fd);
  }
  else{
    system_call(SYS_WRITE,STDOUT,"Invalid number of arguments\n",strlen("Invalid number of arguments\n"));
  }

  return 0;
}
