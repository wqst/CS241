/** @file libscheduler.h
 */

#ifndef LIBSCHEDULER_H_
#define LIBSCHEDULER_H_

#include "../libpriqueue/libpriqueue.h"

/**
  Constants which represent the different scheduling algorithms
*/
typedef enum {FCFS = 0, SJF, PSJF, PRI, PPRI, RR} scheme_t;

typedef struct _job_t {
	int id;
	int priority;
	int time_remaining;
	int time_received;
	int time_responded;	//defined as the time it took to be responded to, not the absolute time at which it was responded to
	int time_responded_to;	//used so we remember when we responded to the job to fix an edge case
	int time_updated;	//set to the current time whenever time_remaining is changed or the job is placed on a core
	int time_waited;
} job_t;

typedef struct _scheduler_t {
	scheme_t scheme;
	priqueue_t queue;
	int cores;
	job_t** core_status;	//matrix with length cores-1 that stores a pointer to the job on each core
} scheduler_t;

int   compare_fcfs                     (const void * a, const void * b);
int   compare_sjf                      (const void * a, const void * b);
int   compare_pri                      (const void * a, const void * b);
void  update_cores                     ();
void  statify                          (job_t* job, int time);
void  scheduler_start_up               (int cores, scheme_t scheme);
int   scheduler_should_preempt         (job_t* current_job, job_t* new_job, scheme_t scheme);
int   scheduler_new_job                (int job_number, int time, int running_time, int priority);
int   scheduler_job_finished           (int core_id, int job_number, int time);
int   scheduler_quantum_expired        (int core_id, int time);
float scheduler_average_turnaround_time();
float scheduler_average_waiting_time   ();
float scheduler_average_response_time  ();
void  scheduler_clean_up               ();

void  scheduler_show_queue             ();

#endif /* LIBSCHEDULER_H_ */
