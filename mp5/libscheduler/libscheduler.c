/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"


/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements. 
*/

scheduler_t* sched;

int finished_jobs;
float avg_wait, avg_turnaround, avg_response;

//putting all the compare functions up here, don't mind me

int compare_fcfs(const void * a, const void * b) {	//this is also used for RR since RR is just FCFS with a quantum
	return 0;
}

int compare_sjf(const void * a, const void * b) {	//also used for PSJF because they both share their queueing method
	int num = ((job_t*)a)->time_remaining - ((job_t*)b)->time_remaining;
	if(num != 0)
		return num;
	return ((job_t*)a)->time_received - ((job_t*)b)->time_received;	//time received is to be used as tiebreaker if priorities are equal
}

int compare_pri(const void * a, const void * b) {	//also used for PPRI because they both share their queueing method
	int num = ((job_t*)a)->priority - ((job_t*)b)->priority;
	if(num != 0)
		return num;
	return ((job_t*)a)->time_received - ((job_t*)b)->time_received;	//time received is to be used as tiebreaker if priorities are equal
}

void update_cores(int time) {
	int i;
	for(i = 0; i < sched->cores; i++) {
		if(sched->core_status[i] != NULL) {
			sched->core_status[i]->time_remaining -= (time - sched->core_status[i]->time_updated);	//updates remaining time
			sched->core_status[i]->time_updated = time;
		}
	}
}

void statify(job_t* job, int time) {
	printf("statifying job %d. wait: %d turnaround: %d response: %d\n", job->id, job->time_waited, time - job->time_received, job->time_responded);
	avg_wait = (avg_wait * (float)finished_jobs + (float)(job->time_waited))/((float)finished_jobs + 1.0f);
	avg_turnaround = (avg_turnaround * (float)finished_jobs + (float)time - (float)(job->time_received))/((float)finished_jobs + 1.0f);
	avg_response = (avg_response * (float)finished_jobs + (float)(job->time_responded))/((float)finished_jobs + 1.0f);
	finished_jobs++;
}

/**
  Initalizes the scheduler.
 
  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme) {
	sched = malloc(sizeof(scheduler_t));
	sched->scheme = scheme;

	if(scheme == FCFS || scheme == RR)
		priqueue_init(&sched->queue, &compare_fcfs);
	if(scheme == SJF || scheme == PSJF)
		priqueue_init(&sched->queue, &compare_sjf);
	if(scheme == PRI || scheme == PPRI)
		priqueue_init(&sched->queue, &compare_pri);

	sched->cores = cores;
	sched->core_status = malloc(sizeof(job_t*) * (sched->cores));
	int i;
	for(i = 0; i < cores; i++)
		sched->core_status[i] = NULL;
	finished_jobs = 0;
	avg_wait = avg_turnaround = avg_response = 0.0f;
}

int scheduler_should_preempt(job_t* current_job, job_t* new_job, scheme_t scheme) {
	
	int diff = -1;
	if(scheme == PSJF) {
		diff = current_job->time_remaining - new_job->time_remaining;
		if(diff < 0)	//the current job is shorter
			return 0;
		if(diff > 0)
			return 1;
		return current_job->time_received > new_job->time_received;
	}
	if(scheme == PPRI) {
		diff = current_job->priority - new_job->priority;
		if(diff < 0)	//the current job is shorter
			return 0;
		if(diff > 0)
			return 1;
		return current_job->time_received > new_job->time_received;
	}
	return diff;
}

/**
  Called when a new job arrives.
 
  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made. 
 
 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
	//make a new job_t
	job_t* new = malloc(sizeof(job_t));
	new->id = job_number;
	new->priority = priority;
	new->time_remaining = running_time;
	new->time_received = time;
	new->time_responded = -1;
	new->time_responded_to = -1;
	new->time_updated = time;
	new->time_waited = 0;
	//first we should update the time remaining on each task
	update_cores(time);
	//then we'll loop over them again, looking for an empty core to toss it on
	int i;
	for(i = 0; i < sched->cores; i++) {
		if(sched->core_status[i] == NULL) {
			//this core is empty; let's set everything up for the new job and assign it
			new->time_responded = 0;
			new->time_responded_to = time;
			sched->core_status[i] = new;
			return i;
		}
	}
	//now we need to go over the cores a third time for preemptive schedulers, looking for a task we can preempt
	if(sched->scheme == PSJF || sched->scheme == PPRI) {
		int preempted = -1;
		for(i = 0; i < sched->cores; i++) {
			//should this job preempt the one currently there?
			if(scheduler_should_preempt(sched->core_status[i], new, sched->scheme)) {
				//is this the "worst" out of all cores so far?
				if(preempted == -1 || scheduler_should_preempt(sched->core_status[i], sched->core_status[preempted], sched->scheme))
					preempted = i;
			}
		}
		if(preempted != -1) {
			//if the newly-preempted job was just responded to by a job_finished call in this time slot, we need to reset its time_responded to -1
			if(sched->core_status[preempted]->time_responded_to == time) {
				sched->core_status[preempted]->time_responded_to = -1;
				sched->core_status[preempted]->time_responded = -1;
			}
			//this task has preempted another, so we'll take the old one off the core, throw it in the queue, and replace it with the new one
			priqueue_offer(&sched->queue, sched->core_status[preempted]);
			//printf("Job %d has preempted job %d on core %d\n", job_number, sched->core_status[preempted]->id, preempted);
			sched->core_status[preempted] = new;
			new->time_responded = 0;
			new->time_responded_to = time;
			return preempted;
		}
	}	//this task can't preempt any other, so it's going on the queue
	//at this point, no cores are free. we'll stick this one on the queue according to the scheduling algorithm
	priqueue_offer(&sched->queue, new);
	return -1;
}


/**
  Called when a job has completed execution.
 
  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.
 
  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
	//first we'll update the cores with the current time
	update_cores(time);
	//now we'll take the finished job off its core, gather its stats, and free it
	statify(sched->core_status[core_id], time);
	free(sched->core_status[core_id]);

	if(priqueue_size(&sched->queue) != 0) {
		sched->core_status[core_id] = priqueue_poll(&sched->queue);
		sched->core_status[core_id]->time_waited += (time - sched->core_status[core_id]->time_updated);
		if(sched->core_status[core_id]->time_responded == -1) {
			sched->core_status[core_id]->time_responded = (time - sched->core_status[core_id]->time_received);
			sched->core_status[core_id]->time_responded_to = time;
		}
		sched->core_status[core_id]->time_updated = time;
		return sched->core_status[core_id]->id;
	}
	sched->core_status[core_id] = NULL;
	return -1;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.
 
  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator. 
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	//first we'll update the job on that core
	sched->core_status[core_id]->time_remaining -= time - sched->core_status[core_id]->time_updated;
	sched->core_status[core_id]->time_updated = time;
	//printf("quantum expired on core %d, that task has %d time left\n", core_id, sched->core_status[core_id]->time_remaining);
	//then we'll decide if we should remove it
	if(sched->core_status[core_id]->time_remaining <= 0) {
		//if so, statify and free it
		statify(sched->core_status[core_id], time);
		free(sched->core_status[core_id]);
		sched->core_status[core_id] = NULL;
	}
	else {	//if the job hasn't expired, we'll just put it on the queue
		priqueue_offer(&sched->queue, sched->core_status[core_id]);
		sched->core_status[core_id] = NULL;
	}
	//then check to see if there are any jobs in the queue to replace it
	if(priqueue_size(&sched->queue) != 0) {
		job_t* add = priqueue_poll(&sched->queue);
		add->time_waited += time - add->time_updated;
		if(add->time_responded == -1) {
			add->time_responded = time - add->time_received;
			add->time_responded_to = time;
		}
		add->time_updated = time;
		sched->core_status[core_id] = add;
		return add->id;
	}
	return -1;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	return avg_wait;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return avg_turnaround;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return avg_response;
}


/**
  Free any memory associated with your scheduler.
 
  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
	priqueue_destroy(&sched->queue);
	free(sched->core_status);
	free(sched);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)  
  
  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
	//go through the queue, reading the job id and priority/time remaining
	priqueue_item_t* iter = sched->queue.head;
	int i;
	for(i = 0; i < sched->queue.length; i++) {
		if(sched->scheme == PRI || sched->scheme == PPRI)
			printf("%d(priority %d) ", ((job_t*)(iter->item))->id, ((job_t*)(iter->item))->priority);
		else
			printf("%d(time_remaining %d) ", ((job_t*)(iter->item))->id, ((job_t*)(iter->item))->time_remaining);
		iter = iter->next;
	}
}
