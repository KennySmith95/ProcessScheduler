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
char * progname = NULL;

int main(int argc, char* argv[]){
	//Setting up the variable
	int nprocesses, i,  index = 0,count = 0;
	char  *command, **cmdargs, *p;
	struct timeval tv;
	char nprocs[] = "--number=",  comnd[] = "--command=";

	//Checking the number of arguments provided
	//nprocessors isn't used until thv3
	if(argc < 2){
		p1perror(2, "usage: <progname> [--number=<nprocesses>] [--nprocessors=<nprocessors>] --command=<command>");
		exit(EXIT_FAILURE);
	}
	progname = argv[0];

	//If the environment variable are set grab values from environment variables
	if((p = getenv("TH_NPROCESSES"))!= NULL)
		nprocesses = p1atoi(p);

	//alloc space for command args
	if((cmdargs = (char **)malloc(50*sizeof(char*))) == NULL){
		p1perror(2, "malloc failed");
		exit(EXIT_FAILURE);
	}

	//For each argument provided check to see which argument it corresponds to 
	//Override the environment variable if command line arg is provided
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
	
	// Create an array of process ids	
	pid_t pid[nprocesses];

	//start timer
	gettimeofday(&tv,NULL);
	time_t start = tv.tv_usec;

	//fork for each process and execute the command
	for (count = 0; count < nprocesses; count++){
		if((pid[count] = fork())==0){
			//exec command on each process
			execvp(cmdargs[0],cmdargs);
			p1perror(2, "error executing command");
		}
		else if (pid[count] < 0){
			p1perror(2, "fork failed");
		}
	}
	
	//wait for child processes to terminate
	for (count = 0; count < nprocesses; count++){
		wait(&pid[count]);
	}


	//end timer
	gettimeofday(&tv,NULL);
	time_t end = tv.tv_usec;
	time_t elapsed = end - start;
	p1putstr(1, "elaspsed time: ");
	p1putint(1, elapsed);
	p1putstr(1, "ms\n");

	//Free allocated arguments
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
