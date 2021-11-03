#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char** argv) {
  char buffer [32];
  int fileDescriptors[2];
  char* msg = "piping";
  pid_t pid;

  //cleaning the buffer
  for (int i=0; i<32; i++){buffer[i] = 0;}

  if (pipe(fileDescriptors) == -1){
      int error_code = errno;
      perror(strerror(error_code));
      exit(EXIT_FAILURE);
  }

//condition for the child proccess
  if (!(pid = fork())){    
    close (fileDescriptors[0]);
    write(fileDescriptors[1],msg, strlen(msg));
  }
  //condition for the parent process
  else {
    close(fileDescriptors[1]);
    read(fileDescriptors[0], buffer, strlen(msg));
    printf("%s\n", buffer);
  }
  return 0;
}
