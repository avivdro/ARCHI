#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>


void signalHandler(int sig){
		printf("Looper received : %s\n",strsignal(sig));
		if(sig==SIGTSTP){printf("Stopping the process\n");}
		if(sig==SIGCONT){printf("Continuting the process\n");}
		if(sig==SIGINT){printf("Interrupting the process\n");}
		signal(sig,SIG_DFL);
		raise(sig);
		if(sig==SIGTSTP){signal(SIGCONT,signalHandler);}
		if(sig==SIGCONT){signal(SIGTSTP,signalHandler);}
}


int main(int argc, char **argv){

	printf("Looper - start.\n");
	signal(SIGINT,signalHandler);
	signal(SIGCONT,signalHandler);
	signal(SIGTSTP,signalHandler);
	while(1) {
		sleep(2);
	}

	return 0;
}
