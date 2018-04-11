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
#include <fcntl.h>

#define UNUSED __attribute__((unused))

//Setup process struct
typedef struct process{
	struct process *next;
	int started;
	pid_t pid;
} pr;

//Setup process Queue
typedef struct pQueue{
	pr *head;
	pr *tail;
	int proc;
	int prcs;
}pQ;

//Set up global file variables
pid_t *pid;
pQ running, waiting;
int finCount, *finishedPID;
char **cmdargs;
int globCount=0;

/* execTime()
 * Input: path to the process id information
 * Returns: void
 * Descriptions: This function prints information to the command line associated with the
 * execution time of each process
 */
void execTime(char path[1024]);

/* ioTime()
 * Input: path to the process id information
 * Returns: void
 * Descriptions: This function prints information to the command line associated with the
 * input/output time of each process
 */
void ioTime(char path[1024]);


/* memUsed()
 * Input: path to the process id information
 * Returns: void
 * Descriptions: This function prints information to the command line associated with the
 * memory used by the processes
 */
void memUsed(char path[1024]);

/* readProc()
 * Input: Process ID
 * Output: void
 * Description: finds the associated directory for the given process ID so that it can call
 * the associated functions to print input output time, memory used, and execution time of
 * the process
 */
void readProc(pid_t pida);

/* createPr()
 * Input: process ID
 * Returns: pointer to process struct
 * Description: takes in a process ID, creates a process struct and returns a pointer to the process.
 */
pr *createPr(int pid);

/* initQ()
 * Input: void
 * Returns: void 
 * Description: Initializes waiting and running process queue
 */
void initQ();

/* push() 
 * Input: pointer to Queue and pointer to Process
 * Returns: void
 * Description: enqueues a process to the given Queue
 */
void push(pQ *queue, pr *proc);

/* pop()
 * Input: pointer to queue
 * Returns: Pointer to first process in queue
 * Description: Dequeues process from the given queue and returns the pointer
 */
pr * pop(pQ *queue);

/* stop()
 * Input: void
 * Returns: void
 * Description: the process scheduler calls this to stop processes and move processes from the
 * running queue to the waiting queue. If the process completes in the time alloted then it is 
 * removed from the queues.
 */
void stop();

/* run()
 * Input: void
 * Returns: void
 * Description: the process scheduler calls this to move processes from the waiting queue 
 * to the running queue, the processes are also started in this function
 */
void run();
