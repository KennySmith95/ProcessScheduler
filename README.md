This Repository is a four part project that was completed for my Computer Science 415 class, Operating Systems.

The program is broken into 4 parts, each builds on the previous part.

The goal of this project is to create a process scheduler utilizing Linux system calls. 
The scheduler creates processes by forking off the parent, then uses a timer to schedule 
these processes onto the processor. It also monitors the processes memory usage, input output 
time and execution time. We were not allowed to use C library calls such as printf, fgets, 
system, strlen, etc. So p1fxns.c/h was used(provided by professor) for these calls so that 
we understood how they actually worked. 

Test Harness Version (THV)
# THV1 

In THV1 the program sets up the basic components to the process scheduler. It takes in 
environment variables or command line arguments to set number of processes. The command line 
also takes in number of processors but number of processors isn't used until THV3 so the 
variable isn't handle in thv1 and thv2 to avoid unused variable warnings. The program then 
creates the number of processes by forking from the parent. The process then runs until 
completion and when all children finish the parent cleans up and terminates.

#THV2

THV2 tests all the functionality that will be necessary for the process scheduler without
actually scheduling anything. It tests starting a process, stopping a process, and continuing 
a process.

In THV2 the functionality of thv1 remains, what is added however, is after a process is
created it doesn't run until completion. When the new process starts it is immediately put to 
sleep. The process remains idle until the parent sends a SIGUSR1 signal to its child to wake 
the process. After the signal is received by the child the child runs until it receives the 
SIGSTOP signal from the parent. Then it stays idle until it receives a SIGCONT signal from 
the parent. After the continue, the processes run until termination and when all children 
finish the parent cleans up and terminates. 

#THV3

THV3 ties all these components together by setting up a timer that starts and 
stops processes.

In THV3 the functionality of thv2 remains but the configuration is changed. Instead of just 
starting and stopping processes once. The processes are added to a running queue and waiting 
queue. All processes start in the waiting queue and as the scheduler runs it moves processes 
from the waiting queue into the running queue and when the time allotted is up returns them 
to the waiting queue if they didn't complete.
 
#THV4

THV4 adds in information tracking for each process that is executed, this was originally created on Ubuntu in 2016 so it's possible that this part doesn't function correctly since file structure could be different.

In THV3 it opens the directory that holds the information for running processes and reads/outputs input/output time, execution time, and memory usage.

