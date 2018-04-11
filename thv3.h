/* Kenny Smith
 * CIS 415
 * Fall 2016
 * Processor Scheduling part 3
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "p1fxns.h"
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
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
pQ running;
pQ waiting;
char **cmdargs;
int finCount;
int *finishedPID;
int globCount=0;

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

/* createPr()
 * Input: process id
 * Returns: pointer to process struct
 * Description: takes in a process ID, creates a process struct and returns a pointer to the process.
 */
pr *createPr(int pid);

/* stopFunc()
 * Input: proced id
 * Returns: void
 * Description: sends stop signal to given process
 */
void stopFunc(pid_t pid);

/* contFunc()
 * Input: proced id
 * Returns: void
 * Description: sends continue signal to given process
 */
void contFunc(pid_t pid);

/* initQ()
 * Input: void
 * Returns: void
 * Description: Initializes waiting and running process queue
 */
void initQ();

/* Push()
 * Input: pointer to Queue and pointer to Process
 * Returns: void 
 * Description: Adds a process to the given Queue
 */
void push(pQ *queue, pr *proc);

/* pop()
 * Input: pointer to queue
 * Return: Pointer to Process
 * Description: Dequeues process from the given Queue and returns the pointer
 */
pr * pop(pQ *queue);
