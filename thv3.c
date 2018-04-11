/* Kenny Smith
 * CIS 415
 * Fall 2016
 * Processor Scheduling part 3
*/

#include "thv3.h"


//SIGCHLD handler function
void sigchild_hdl(UNUSED int signal){
	if(signal == SIGCHLD){
		int pid;
		int result;
		while((pid = waitpid(-1, &result, WNOHANG))>0){
			if(WIFEXITED(result)){
				finishedPID[globCount++] = pid;
			}
		}	 
	
	}
}

//SIGUSR1 handler function
void sigusr1_hdl(UNUSED int signal){
	execvp(cmdargs[0],cmdargs);
}

//SIGALRM handler function handles processor scheduler function
void onalarm(UNUSED int signal){
	if(finCount == waiting.prcs){
		return;
	}
	else{
		stop();
		run();
	}
}

int main(int argc, char* argv[]){
	//Stting up Variables
	int nprocesses, nprocessors, i, index = 0,count = 0;
	char  *command, *p;
	struct timeval tv;
	struct itimerval it_val;
	initQ();

	//Set Process Scheduler timer 
	it_val.it_value.tv_sec = 0;
	it_val.it_value.tv_usec = 250000;
	it_val.it_interval = it_val.it_value;
	
	//Setting up Signal Handlers
	signal(SIGALRM, onalarm);
	signal(SIGUSR1,sigusr1_hdl);
	signal(SIGCHLD, sigchild_hdl);

	//Checking Argument input
	if(argc < 2){
		p1perror(2, "usage: <progname> [--number=<nprocesses>] [--nprocessors=<nprocessors>] --command=<command>");
		exit(EXIT_FAILURE);
	}

	//Check environment variables for number of processors and processes
	if((p = getenv("TH_NPROCESSES"))!= NULL)
		nprocesses = p1atoi(p);
	if((p = getenv("TH_NPROCESSORS"))!= NULL)
		nprocessors = p1atoi(p);
	char nprocs[] = "--number=", npors[] = "--processors=", comnd[] = "--command=";
	if((cmdargs = (char **)malloc(50*sizeof(char*))) == NULL){
		p1perror(2, "malloc failed");
		exit(EXIT_FAILURE);
	}
	
	//check each arguments prefix to se which variable to set
	//If command line variables are provided it overrides environment variables
	for(i = 1; i < argc; i++){
		if(p1strneq(argv[i], nprocs, p1strlen(nprocs)))
			nprocesses = p1atoi(&argv[i][p1strlen(nprocs)]);
		else if(p1strneq(argv[i], npors, p1strlen(npors)))
			nprocessors = p1atoi(&argv[i][p1strlen(npors)]);
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

	//Count of finished processes
	finCount = 0;		

	//Create an array to keep track of process ids
	pid_t pid[nprocesses];

	//Get start time
	gettimeofday(&tv,NULL);
	finishedPID = malloc(sizeof(int)*nprocesses);
	
	//Setting the number of processors and processes in the waiting Queue
	waiting.proc = nprocessors;
	waiting.prcs = nprocesses;
	time_t start = tv.tv_usec;
	
	//for the number of processes fork and put the process to sleep until signal is called
	for (count = 0; count < nprocesses; count++){
		if((pid[count] = fork())==0){
			while(1)
				pause();
		}
		else if (pid[count] < 0){
			p1perror(2, "fork failed");
		}
	}

	//for each process create a process struct and push it to the waiting queue for scheduling
	for (count = 0; count < nprocesses; count++){
		pr* temp = createPr(pid[count]);
		push(&waiting, temp);
	}

	//set timer for processor scheduling
	if(setitimer(ITIMER_REAL,&it_val,NULL) == -1){
		p1perror(2, "error setting timer");
		exit(EXIT_FAILURE);
	}
	
	//Wait until all processes are finished
	while(finCount != nprocesses)
		pause();

	//check to make sure all processes are finished and wait until they are
	for(i = 0; i < nprocesses; i++){
		wait(&pid[i]);
	}

	//Get end time and calculate elapsed time
	gettimeofday(&tv,NULL);
	time_t end = tv.tv_usec;
	time_t elapsed = end - start;

	//output elapsed time
	p1putstr(1, "elaspsed time: ");
	p1putint(1, elapsed);
	p1putstr(1, "ms\n");

	//Free allocated memory
	for(count = 0; count<50; count++){
		if(cmdargs[count] == NULL){
			free(cmdargs[count]);
			break;
		}
		free(cmdargs[count]);
	}
	free(finishedPID);
	free(cmdargs);
	return 1;
}
pr *createPr(int pid){
	pr *proc = malloc(sizeof(pr));
	proc->pid = pid;
	proc->next = NULL;
	proc->started = 0;
	return proc;
}
void stop(){
	int test = 0;
	//While processes are still running (still in running queue) pop and kill each process
	while(running.head != NULL){
		test = 0;
		int i;

		//pop a process from the running queue and stop it
		pr * temp = pop(&running);
		kill(temp->pid,SIGSTOP);

		//check to see if the process had completed, if so free memory and increment the counter
		for(i = 0; i < waiting.prcs; i++){
			if(temp->pid == finishedPID[i]){
				free(temp);
				finCount++;
				test = 1;
				break;
			}
		}
		
		//if it didn't finish add it to the waiting queue so that it can be rescheduled
		if(!test){
		push(&waiting,temp);
		}
	}
	 
	return;
}


void run(){
	int i;

	//For each process in the waiting queue 	
	for(i = 0; i<waiting.proc; i++){

		//If the waiting queue is empty return
		if(waiting.head == NULL){
			return;
		}

		//Get process from the waiting queue
		pr * temp = pop(&waiting);
		//if the process hasn't been ran set started flag, push to running queue and send SIGUSR1
		if(temp->started == 0){
			temp->started = 1;
			push(&running, temp);
			kill(temp->pid,SIGUSR1);
		}
		//if it's already started send SIGCONT and add to running queue
		else{
			push(&running, temp);
			kill(temp->pid,SIGCONT);
		}
	}
}

//send stop signal to given process
void stopFunc(pid_t pid){
	kill(pid, SIGSTOP);
}

//send continue signal to given process
void contFunc(pid_t pid){
	kill(pid, SIGCONT);
}
	
//get process from the front of the queue
pr* pop(pQ *queue){
	pr *temp = queue->head;
	if (temp->next == NULL)	{
		queue->head = NULL;
		queue->tail = NULL;
		return temp;
	}
	else
		queue->head = queue->head->next;

	return temp;
}

//enqueue process in the queue
void push(pQ *queue, pr * proc){
	if(queue->head == NULL){
		queue->head = proc;
		queue->head->next = NULL;
		queue->tail = proc;
		return;
	}
	proc->next = NULL;
	queue->tail->next = proc;
	queue->tail = proc;
	return;
}

//Initialize values of running and waiting Queue
void initQ(){
	running.head = NULL;
	running.tail = NULL;
	waiting.head = NULL;
	waiting.tail = NULL;
	running.proc = 0;
	waiting.proc = 0;
}
