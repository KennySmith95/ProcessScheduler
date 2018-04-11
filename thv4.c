/* Kenny Smith
 * CIS 415
 * Fall 2016
*/
#include "thv4.h"

//SIGCHLD signal handler function
void sigchild_hdl(UNUSED int signal){
	if(signal == SIGCHLD){
		int pid, result;
		while((pid = waitpid(-1, &result, WNOHANG))>0){
			if(WIFEXITED(result)){
				finishedPID[globCount++] = pid;
			}
		}	 
	
	}
}

//SIGUSR1 signal handler function
void sigusr1_hdl(UNUSED int signal){
	execvp(cmdargs[0],cmdargs);
}

//SIGALRM signal handler function
void onalarm(UNUSED int signal){
	if(finCount != waiting.prcs){
		stop();
		run();
	}
}

int main(int argc, char* argv[]){
	//Setting up variables
	int nprocesses, nprocessors, i, index = 0,count = 0;
	char  *command, *p, nprocs[] = "--number=", npors[] = "--processors=", comnd[] = "--command=";
	struct timeval tv;
	struct itimerval it_val;

	//Initializing pending and running process Queue
	initQ();

	//Setting up signal handlers
	signal(SIGALRM, onalarm);
	signal(SIGUSR1,sigusr1_hdl);
	signal(SIGCHLD, sigchild_hdl);

	//Setting up timer for SIGALRM
	it_val.it_value.tv_sec = 0;
	it_val.it_value.tv_usec = 1000;
	it_val.it_interval = it_val.it_value;

	//Checking number of arguments number and nprocessors are optional
	if(argc < 2){
		p1perror(2, "usage: <progname> [--number=<nprocesses>] [--nprocessors=<nprocessors>] --command=<command>");
		exit(EXIT_FAILURE);
	}

	//Get variables from environment variables if available
	if((p = getenv("TH_NPROCESSES"))!= NULL)
		nprocesses = p1atoi(p);
	if((p = getenv("TH_NPROCESSORS"))!= NULL)
		nprocessors = p1atoi(p);
	if((cmdargs = (char **)malloc(50*sizeof(char*))) == NULL){
		p1perror(2, "malloc failed");
		exit(EXIT_FAILURE);
	}

	//For each argument check which argument the value is for
	//command line arguments override environment variables
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
	
	//set the count of finished processes to 0
	finCount = 0;		

	//create an array of process ids to keep track of completed processes
	pid = (pid_t *)malloc(sizeof(pid_t)*nprocesses);

	finishedPID = malloc(sizeof(int)*nprocesses);

	//Set number of processors and waiting processes
	waiting.proc = nprocessors;
	waiting.prcs = nprocesses;

	//Get Start time
	gettimeofday(&tv,NULL);
	time_t start = tv.tv_usec;

	//create a process for number of processes then have the process wait until a signal is sent
	for (count = 0; count < nprocesses; count++){
		if((pid[count] = fork())==0){
			while(1)
				pause();
		}
		else if (pid[count] < 0){
			p1perror(2, "fork failed");
		}
	}

	//for every process create a process struct and add it to the waiting queue
	for (count = 0; count < nprocesses; count++){
		pr* temp = createPr(pid[count]);
		push(&waiting, temp);
	}

	//Set timer for SIGALRM
	if(setitimer(ITIMER_REAL,&it_val,NULL) == -1){
		p1perror(2, "error setting timer");
		exit(EXIT_FAILURE);
	}

	//while the processes aren't finished parent waits
	while(finCount != nprocesses)
		pause();

	//after finCount == the number of processes, it waits to make sure the process has terminated
	for(i = 0; i < nprocesses; i++){
		wait(&pid[i]);
	}

	//get end time and calculate the elapsed time
	gettimeofday(&tv,NULL);
	time_t end = tv.tv_usec;
	time_t elapsed = end - start;

	//output the elapsed time
	p1putstr(1, "elaspsed time: ");
	p1putint(1, (int) elapsed);
	p1putstr(1, "ms\n");

	//free allocated memory
	for(count = 0; count<50; count++){
		if(cmdargs[count] == NULL){
			free(cmdargs[count]);
			break;
		}
		free(cmdargs[count]);
	}
	free(finishedPID);
	free(cmdargs);
	free(pid);
	return 1;
}

//Create a process struct and return a pointer to it
pr *createPr(int tempPid){
	pr *proc = malloc(sizeof(pr));
	proc->pid = tempPid;
	proc->next = NULL;
	proc->started = 0;
	return proc;
}

//get the information about the process id in the running processes directory
void readProc(pid_t pidT){
	int i;
	for(i = 0; i < waiting.prcs; i++){
		if(pidT == finishedPID[i]){
			return;
		}
	}
	char *dir = "/proc/", endingChar[1024] = {"\0"};
	char apid[64] = {"\0"};
	p1itoa(pidT, apid);
	p1putstr(1, "\nInformation for process number ");
	p1putstr(1, apid);
	p1putstr(1, ":\n");
	
	p1strcat(endingChar,dir);
	p1strcat(endingChar,apid);
	char *tempEnding = p1strdup(endingChar);
	char *tempEnding2 = p1strdup(endingChar);
	ioTime(endingChar);
	execTime(tempEnding);
	memUsed(tempEnding2);
	p1putstr(1,"\n");
	free(tempEnding);
	free(tempEnding2);

}

//Output info about the memory each process used
void memUsed(char path[1024]){
	char *file = "/smaps", *stack = "[stack]", *heap = "[heap]", pathBuf[1024] = {"\0"}, buffer[1024];
	int index, fd;
	p1strcat(path,file);
	p1strcat(path,pathBuf);
	if((fd = open(path,O_RDONLY))== -1){
		p1perror(2, "error opening file");
		exit(EXIT_FAILURE);
	}
	while(p1getline(fd, buffer,1024)>0){
		index = p1strchr(buffer, '[');
		if(p1strneq(stack, &buffer[index], p1strlen(stack))){
			p1putstr(1, "Stack Memory Segment:\n");
			p1putstr(1, buffer);
			p1putstr(1, "Size of Stack Memory: \n");
			p1getline(fd,buffer,1024);
			p1putstr(1,buffer);
			}	
		if(p1strneq(heap, &buffer[index], p1strlen(heap))){
			p1putstr(1, "Heap Memory Segment:\n");
			p1putstr(1, buffer);
			p1putstr(1, "Size of Heap Memory: \n");
			p1getline(fd,buffer,1024);
			p1putstr(1,buffer);
			}
	}

		

}

//Output information about execution time of processes
void execTime(char path[1024]){
	char *file = "/sched", *exec = "se.sum_exec_runtime", pathBuf[1024] = {"\0"}, buffer[1024], word[50]; 
	int index, fd;
	p1strcat(path,file);
	p1strcat(path, pathBuf);
	if((fd = open(path,O_RDONLY))== -1){
		p1perror(2, "error opening file");
		exit(EXIT_FAILURE);
	}
	while(p1getline(fd, buffer,1024)>0){
		index = p1getword(buffer, 0, word);
		if(p1strneq(exec, word, index -1)){
			p1putstr(1,"Process execution time:");
			p1putstr(1,buffer);
		}
	}
	close(fd);

}

//Output information on the time spent doing input and output
void ioTime(char path[1024]){
	char *file = "/io", *read = "read_bytes:", *write = "write_bytes:", pathBuf[1024] = {"\0"}, buffer[1024], word[50];
	int index, fd;
	p1strcat(path,file);
	p1strcat(path, pathBuf);
	if((fd = open(path,O_RDONLY))== -1){
		p1perror(2, "error opening file");
		exit(EXIT_FAILURE);
	}
	while(p1getline(fd, buffer,1024)>0){
		index = p1getword(buffer, 0, word);
		if(p1strneq(read, word, index-1)){
			p1putstr(1,"Bytes read:\n");
			p1putstr(1,buffer);
		}
		else if(p1strneq(write, word, index-1)){
			p1putstr(1,"Bytes written:\n");
			p1putstr(1,buffer);
			return;
		}
	}
	close(fd);
			
}

//This function is called by the process schedule timer to stop all running processes and move them to waiting queue if incomplete
void stop(){
	int i, test = 0;
	while(running.head != NULL){
		test = 0;
		pr * temp = pop(&running);
		kill(temp->pid,SIGSTOP);
		for(i = 0; i < waiting.prcs; i++){
			if(temp->pid == finishedPID[i]){
				free(temp);
				finCount++;
				test = 1;
				break;
			}
		}
		if(!test){
		push(&waiting,temp);
		}
	}
	 
	return;
}

//This function is called by the process schedule timer to start processes sitting in the waiting queue
void run(){
	int i;
	for(i = 0; i<waiting.proc; i++){
		if(waiting.head == NULL){
			return;
		}
		pr * temp = pop(&waiting);
		if(temp->started == 0){
			temp->started = 1;
			push(&running, temp);
			kill(temp->pid,SIGUSR1);
		}
		else{
			push(&running, temp);
			readProc(temp->pid);
			kill(temp->pid,SIGCONT);
		}
	}
}

//This function dequeues the front of the given queue and returns a pointer to the dequeued element
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

//This function enqueues an element to the given queue
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

//This function initializes the waiting and running queues
void initQ(){
	running.head = NULL;
	running.tail = NULL;
	waiting.head = NULL;
	waiting.tail = NULL;
	running.proc = 0;
	waiting.proc = 0;
}
