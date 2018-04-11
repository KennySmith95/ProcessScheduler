/* Kenny Smith
 * CIS 415
 * Fall 2016
*/


#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "p1fxns.h"
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>

#define UNUSED __attribute__((unused))
static char * progname = NULL;
char **cmdargs;
int stopFunc(pid_t pid);
int contFunc(pid_t pid);
void sigusr1_hdl(UNUSED int signal){
	execvp(cmdargs[0],cmdargs);
}
int main(int argc, char* argv[]){
	//Set Variables
	int nprocesses, i, index = 0,count = 0;
	char  *command, *p;
	struct timeval tv;

	//Set signal handlr for SIGUSR1
	signal(SIGUSR1,sigusr1_hdl);

	//Check the number of arguments number and nprocessors are optional
	if(argc < 2){
		p1perror(2, "usage: <progname> [--number=<nprocesses>] [--nprocessors=<nprocessors>] --command=<command>");
		exit(EXIT_FAILURE);
	}
	//Store the project name
	progname = argv[0];

	//If environment variables are set number of processors and processes
	if((p = getenv("TH_NPROCESSES"))!= NULL)
		nprocesses = p1atoi(p);
	char nprocs[] = "--number=",  comnd[] = "--command=";

	//Allocate memory for the commands
	if((cmdargs = (char **)malloc(50*sizeof(char*))) == NULL){
		p1perror(2, "malloc failed");
		exit(EXIT_FAILURE);
	}

	//For each argument provided check which argument it is associated to and set it
	//Command line arguments override environment variables
	for(i = 1; i < argc; i++){
		if(p1strneq(argv[i], nprocs, p1strlen(nprocs)))
			nprocesses = p1atoi(&argv[i][p1strlen(nprocs)]);
		else if(p1strneq(argv[i],comnd, p1strlen(comnd))){	
					while(index != -1){
						if (( command = (char *) malloc(50*sizeof(char *))) == NULL){
							p1perror(2, "malloc failed");
							exit(EXIT_FAILURE);
						}
						index = p1getword(argv[i]+p1strlen(comnd), index, command);
						cmdargs[count] = command;
						if(index!=-1){
							count++;
						}
						else{
							free(cmdargs[count]);
							break;
						}
					}	
					cmdargs[count] = NULL;
		}
	}
	
	//make an array of process ids to keep track of termination	
	pid_t pid[nprocesses];

	//get start time
	gettimeofday(&tv,NULL);
	time_t start = tv.tv_usec;
	//fork each process then put the process to sleep until signal wakes it
	for (count = 0; count < nprocesses; count++){
		if((pid[count] = fork())==0){
				while(1)
					pause();
		}
		else if (pid[count] < 0){
			p1perror(2, "fork failed");
		}
	}
	
	//for each process send signal SIGUSR1
	for (count = 0; count < nprocesses; count++)
		kill(pid[count], SIGUSR1);	

	//for each process send stop signal
	for (count = 0; count < nprocesses; count++){
		stopFunc(pid[count]);	
	}

	//for each process send continue signal
	for (count = 0; count < nprocesses; count++){
		contFunc(pid[count]);
	}

	//wait for each process to terminate
	for (count = 0; count < nprocesses; count++)
		wait(&pid[count]);

	//get end time and calculate elapsed time
	gettimeofday(&tv,NULL);
	time_t end = tv.tv_usec;
	time_t elapsed = end - start;

	//output elapsed time information
	p1putstr(1, "elaspsed time: ");
	p1putint(1, elapsed);
	p1putstr(1, "ms\n");

	//Free cmdarg memory
	for(count = 0; count<50; count++){
		if(cmdargs[count] == NULL){
			free(cmdargs[count]);
			break;
		}
		free(cmdargs[count]);
	}
	free(cmdargs);
	return 1;
}

//send stop signal to given process
int stopFunc(pid_t pid){
	return kill(pid, SIGSTOP);
}

//send continue signal to given process
int contFunc(pid_t pid){
	return kill(pid, SIGCONT);
}
