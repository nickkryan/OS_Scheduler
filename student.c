/*
 * student.c
 * Multithreaded OS Simulation for CS 2200, Project 4
 *
 * This file contains the CPU scheduler for the simulation.  
 */

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "os-sim.h"
#include "list.h"


/*
 * current[] is an array of pointers to the currently running processes.
 * There is one array element corresponding to each CPU in the simulation.
 *
 * current[] should be updated by schedule() each time a process is scheduled
 * on a CPU.  Since the current[] array is accessed by multiple threads, you
 * will need to use a mutex to protect it.  current_mutex has been provided
 * for your use.
 */
static pcb_t **current;
static pthread_mutex_t current_mutex;

extern pthread_cond_t empty_ready;


extern list* list_ptr;
static pthread_mutex_t ready_mutex;

int schedule_time = -1;
int idle_val = 0;
int priority = 0;
int cpu_count;


/*
 * schedule() is your CPU scheduler.  It should perform the following tasks:
 *
 *   1. Select and remove a runnable process from your ready queue which 
 *	you will have to implement with a linked list or something of the sort.
 *
 *   2. Set the process state to RUNNING
 *
 *   3. Call context_switch(), to tell the simulator which process to execute
 *      next on the CPU.  If no process is runnable, call context_switch()
 *      with a pointer to NULL to select the idle process.
 *	The current array (see above) is how you access the currently running process indexed by the cpu id. 
 *	See above for full description.
 *	context_switch() is prototyped in os-sim.h. Look there for more information 
 *	about it and its parameters.
 */
static void schedule(unsigned int cpu_id)
{

    pcb_t *tempReady;
    if (list_ptr == NULL || list_ptr->head == NULL)
    {
        idle_val = 1;
        context_switch(cpu_id, NULL, -1);
    }
    else
    {
        tempReady = list_ptr->head->pcb;
        (*tempReady).state = PROCESS_RUNNING;
        pthread_mutex_lock(&current_mutex);
        current[cpu_id] = tempReady;
        pthread_mutex_unlock(&current_mutex);


        pthread_mutex_lock(&ready_mutex);
        delete_from_list(tempReady);
        pthread_mutex_unlock(&ready_mutex);
        context_switch(cpu_id, tempReady, schedule_time);
    }

}


/*
 * idle() is your idle process.  It is called by the simulator when the idle
 * process is scheduled.
 *
 * This function should block until a process is added to your ready queue.
 * It should then call schedule() to select the process to run on the CPU.
 */
extern void idle(unsigned int cpu_id)
{
    while(idle_val)
    {
        pthread_mutex_lock(&ready_mutex);
        pthread_cond_wait(&empty_ready, &ready_mutex);
        pthread_mutex_unlock(&ready_mutex);
    }
    schedule(cpu_id);

}


/*
 * preempt() is the handler called by the simulator when a process is
 * preempted due to its timeslice expiring.
 *
 * This function should place the currently running process back in the
 * ready queue, and call schedule() to select a new runnable process.
 */
extern void preempt(unsigned int cpu_id)
{

    pcb_t* currently_running;

    pthread_mutex_lock(&current_mutex);
    currently_running = current[cpu_id];
    pthread_mutex_unlock(&current_mutex);

    (*currently_running).state = PROCESS_READY;
    pthread_mutex_lock(&ready_mutex);
    add_to_list(currently_running, 1);
    pthread_mutex_unlock(&ready_mutex);

    schedule(cpu_id);
}


/*
 * yield() is the handler called by the simulator when a process yields the
 * CPU to perform an I/O request.
 *
 * It should mark the process as WAITING, then call schedule() to select
 * a new process for the CPU.
 */
extern void yield(unsigned int cpu_id)
{

    pthread_mutex_lock(&current_mutex);
    pcb_t* process = current[cpu_id];
    pthread_mutex_unlock(&current_mutex);
    (*process).state = PROCESS_WAITING;
    schedule(cpu_id);
}


/*
 * terminate() is the handler called by the simulator when a process completes.
 * It should mark the process as terminated, then call schedule() to select
 * a new process for the CPU.
 */
extern void terminate(unsigned int cpu_id)
{
    pthread_mutex_lock(&current_mutex);
    pcb_t* process = current[cpu_id];
    pthread_mutex_unlock(&current_mutex);
    (*process).state = PROCESS_TERMINATED;
    schedule(cpu_id);
}


/*
 * wake_up() is the handler called by the simulator when a process's I/O
 * request completes.  It should perform the following tasks:
 *
 *   1. Mark the process as READY, and insert it into the ready queue.
 *
 *   2. If the scheduling algorithm is static priority, wake_up() may need
 *      to preempt the CPU with the lowest priority process to allow it to
 *      execute the process which just woke up.  However, if any CPU is
 *      currently running idle, or all of the CPUs are running processes
 *      with a higher priority than the one which just woke up, wake_up()
 *      should not preempt any CPUs.
 *	To preempt a process, use force_preempt(). Look in os-sim.h for 
 * 	its prototype and the parameters it takes in.
 */
extern void wake_up(pcb_t *process)
{

    int currently_idle = 0;
    if (process != NULL) {
        (*process).state = PROCESS_READY;

        pthread_mutex_trylock(&ready_mutex);
        add_to_list(process, 1);
         idle_val = 0;
        pthread_cond_signal(&empty_ready);
        pthread_mutex_unlock(&ready_mutex);

        // fprintf(stderr, "end wake\n");
        int priority_val = (*process).static_priority;
        int current_priority_val = priority_val;
        int i, cpu_choice = -1;
        if (priority) {
            for (i = 0; i < cpu_count; i++)
            {
                if (current[i] != NULL && ((*current[i]).static_priority) < current_priority_val)
                {
                    current_priority_val = (*current[i]).static_priority;
                    cpu_choice = i;
                }
                if (current[i] == NULL)
                {
                    currently_idle = 1;
                    break;
                }
            }
            if (cpu_choice != -1 && !currently_idle)
            {
                force_preempt(cpu_choice);
            }
        }
    }
}


/*
 * main() simply parses command line arguments, then calls start_simulator().
 * You will need to modify it to support the -r and -p command-line parameters.
 */
int main(int argc, char *argv[])
{

    /* Parse command-line arguments */
    if (argc < 2)
    {
        fprintf(stderr, "CS 2200 Project 4 -- Multithreaded OS Simulator\n"
            "Usage: ./os-sim <# CPUs> [ -r <time slice> | -p ]\n"
            "    Default : FIFO Scheduler\n"
            "         -r : Round-Robin Scheduler\n"
            "         -p : Static Priority Scheduler\n\n");
        return -1;
    }
    cpu_count = atoi(argv[1]);

    /* FIX ME - Add support for -r and -p parameters*/
    if (argc > 2) {
        if (*(argv[2]+1) == 'r')
        {
            schedule_time = atoi(argv[3]);
        }
        else if (*(argv[2]+1) == 'p')
        {
            priority = 1;
        }
    }
    /* Allocate the current[] array and its mutex */
    current = malloc(sizeof(pcb_t*) * cpu_count);
    assert(current != NULL);
    pthread_mutex_init(&current_mutex, NULL);
    pthread_mutex_init(&ready_mutex, NULL);
    pthread_cond_init(&empty_ready, NULL);
    /* Start the simulator in the library */
    start_simulator(cpu_count);

    return 0;
}


