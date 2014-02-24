/** @file parmake.c */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include "parser.h"
#include "queue.h"
#include "rule.h"
#include <sys/types.h>
#include <sys/stat.h>

typedef enum { READY, IN_PROGRESS, FINISHED } status;

typedef struct {
	rule_t* rule;
	status stat;
	sem_t in_use;
} dependency_t;

dependency_t* current;
queue_t queue;

int readers = 0;
pthread_mutex_t waiting, acc, mutex, bored;
pthread_cond_t condition;
sem_t sem;

void dependency_init(dependency_t* dep) {
	dep->rule = malloc(sizeof(rule_t));
	dep->stat = READY;
	rule_init(dep->rule);
	sem_init(&dep->in_use, 0, 1);
}

void dependency_destroy(dependency_t* dep) {
	rule_destroy(dep->rule);
	free(dep->rule);
	sem_destroy(&dep->in_use);
}

void parsed_new_target(char* target) {
	if(current != NULL)	//not starting on a new rule
		queue_enqueue(&queue, current);
	current = malloc(sizeof(dependency_t));
	dependency_init(current);
	current->rule->target = malloc(sizeof(char) * strlen(target) + 1);
	strcpy(current->rule->target, target);
}

void parsed_new_dependency(char* target, char* dependency) {
	char* add = malloc(sizeof(char) * strlen(dependency) + 1);
	strcpy(add, dependency);
	queue_enqueue(current->rule->deps, add);
}

void parsed_new_command(char* target, char* command) {
	char* add = malloc(sizeof(char) * strlen(command) + 1);
	strcpy(add, command);
	queue_enqueue(current->rule->commands, add);
}

int check_dependencies(queue_t* in, char* rule_name/*, int thread_id*/) {
	//checks to see if any item in this queue is in the jobs queue, returns 1 if all satisfied, 0 if any are not, and 2 if the rule can be marked as completed without being run.
	if(queue_size(in) == 0)
		return 1;
	//we'll need to keep track of any rules that are file names.
	queue_t* file_rules = malloc(sizeof(queue_t));
	queue_init(file_rules);
	int k, l;
	char found;
	char all_files = 1;
	for(k = 0; k < queue_size(in); k++) {
		found = 0;
		for(l = 0; l < queue_size(&queue); l++) {
			dependency_t* curr = (dependency_t*)(queue_at(&queue, l));
			//make sure we're not checking against itself
			if(strcmp(curr->rule->target, rule_name) == 0)
				continue;
			//we need to make sure no other thread is looking at this
			if(strcmp((char*)queue_at(in, k), curr->rule->target) == 0) {
				if(sem_trywait(&curr->in_use) != 0) {
					sem_wait(&curr->in_use);
				}
				found = 1;
				all_files = 0;
				if(curr->stat != FINISHED) {
					sem_post(&curr->in_use);
					queue_destroy(file_rules);
					free(file_rules);
					return 0;
				}
				else
					sem_post(&curr->in_use);
			}
		}
		//if we haven't found the rule yet, as per the spec, the rule is the name of a file.
		if(found == 0)
			queue_enqueue(file_rules, queue_at(in, k));
	}
	//if we have any files, we need to run some tests on them.
	
	//if all dependencies are files...
	if(all_files) {
		//if the name of the rule does not exist as a file on disk, return 1.
		if(access(rule_name, R_OK)) {
			queue_destroy(file_rules);
			free(file_rules);
			return 1;
		}
		//otherwise, if the rule does exist as a file on disk, get the modification times of it and all file dependencies.
		int end = queue_size(file_rules);
		//stat() things
		struct stat st;
		
		stat(rule_name, &st);
		time_t rule_changed = st.st_mtime;
		for(k = 0; k < end; k++) {
			stat((char*)queue_dequeue(file_rules), &st);
			//if the rule file is not the most recently modified file, return 1.
			if(rule_changed < st.st_mtime) {
				queue_destroy(file_rules);
				free(file_rules);
				return 1;
			}
		}
		//if the rule file is the most recently modified file, return 2.
		queue_destroy(file_rules);
		free(file_rules);
		return 2;
	}
	//if only some of the dependencies are files, we return 1 if all other dependencies are met and 0 if not. Since we shoudl already have returned 0 if any failed, we can safely just return 1 here.
	queue_destroy(file_rules);
	free(file_rules);
	return 1;
}

void* make_thread(void* ptr) {
	char skip;
	while(1) {
		//acquire lock here
		
		int prev;
		pthread_mutex_lock(&waiting);
		pthread_mutex_lock(&mutex);
		prev = readers++;
		pthread_mutex_unlock(&mutex);
		if(prev == 0)
			pthread_mutex_lock(&acc);
		pthread_mutex_unlock(&waiting);

		int j;
		int num_ready = 0;
		skip = 0;
		dependency_t* found = NULL;
		for(j = 0; j < queue_size(&queue); j++) {
			dependency_t* fuck = (dependency_t*)queue_at(&queue, j);
			//first thing's first, make sure this thread's the only one checking on this job.
			if(sem_trywait(&fuck->in_use) != 0) {
				int cnt = 0;
				sem_getvalue(&fuck->in_use, &cnt);
				continue;
			}
			
			//check to see if this rule itself is ready to run
			if(fuck->stat != READY) {
				sem_post(&fuck->in_use);
				continue;
			}

			num_ready++;

			int dependency_status = check_dependencies(fuck->rule->deps, fuck->rule->target);

			if(dependency_status == 2) {
				//this rule can be flagged as finished, but nothing needs to be run
				fuck->stat = FINISHED;
				skip = 1;
				//make sure other threads know they can look at this
				sem_post(&fuck->in_use);
				break;
			}

			if(dependency_status == 1) {
				//this rule is ready to be run, so we'll flag it as being run
				fuck->stat = IN_PROGRESS;
				//remember the rule, then break
				found = fuck;
				//make sure other threads know they can look at this
				sem_post(&fuck->in_use);
				break;
			}

			sem_post(&fuck->in_use);
		}

		//release lock here
		pthread_mutex_lock(&mutex);
		readers--;
		if(readers == 0)
			pthread_mutex_unlock(&acc);
		pthread_mutex_unlock(&mutex);

		if(num_ready == 0) {
			break;
		}

		if(skip) {
			skip = 0;
			continue;
		}

		if(found != NULL) {
			for(j = 0; j < queue_size(found->rule->commands); j++) {
				char* cmd = (char*)queue_at(found->rule->commands, j);
				if(system(cmd) != 0)
					exit(1);
			}
			pthread_cond_broadcast(&condition);
			//acquire write lock here
			pthread_mutex_lock(&waiting);
			pthread_mutex_lock(&acc);
			pthread_mutex_unlock(&waiting);
			//change status to finished
			found->stat = FINISHED;
			//release lock
			pthread_mutex_unlock(&acc);
			//notify all other threads
			pthread_cond_broadcast(&condition);
		}
		else {
			//if no job is found, wait at the end until woken up by another thread finishing a job
			pthread_mutex_lock(&bored);
			pthread_cond_wait(&condition, &bored);
			pthread_mutex_unlock(&bored);
		}
	}
	return NULL;
}

/**
 * Entry point to parmake.
 */
int main(int argc, char **argv)
{
	//[Part 1]
	signed char option;
	char* makefile = NULL;
	int num_threads = 1;

	while((option = getopt(argc, argv, "f:j:")) != -1)
		switch(option) {
			case 'f':
			makefile = optarg;
			break;

			case 'j':
			num_threads = atoi(optarg);
			break;	
		}
	int i;
	int target_count = argc - optind;
	char* targets[target_count + 1];
	for(i = optind; i < argc; i++)
		targets[i - optind] = argv[i];
	targets[target_count] = NULL;
	if(makefile == NULL) {
		//check if ./makefile or ./Makefile are here, use them if so
		if(!access("Makefile", R_OK))
			makefile = "Makefile";
		if(!access("makefile", R_OK))
			makefile = "makefile";
		if(makefile == NULL) {
			return 1;
		}
	}
	
	//[Part 2]
	queue_init(&queue);
	parser_parse_makefile(makefile, targets, parsed_new_target, parsed_new_dependency, parsed_new_command);
	queue_enqueue(&queue, current);

	//init mutexes
	pthread_mutex_init(&waiting, NULL);
	pthread_mutex_init(&acc, NULL);
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&bored, NULL);
	
	//init semaphore
	sem_init(&sem, 1, 1);

	//init cond_t
	pthread_cond_init(&condition, NULL);

	//spawn threads
	pthread_t* threads = malloc(sizeof(pthread_t) * num_threads);
	for(i = 0; i < num_threads; i++)
		pthread_create(&threads[i], NULL, make_thread, NULL);

	for(i = 0; i < num_threads; i++)
		pthread_join(threads[i], NULL);

	pthread_mutex_destroy(&waiting);
	pthread_mutex_destroy(&acc);
	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&bored);

	pthread_cond_destroy(&condition);
	fflush(stdout);

	int length = queue_size(&queue);
	for(i = 0; i < length; i++) {
		dependency_t* dep = queue_dequeue(&queue);
		
		queue_t* derp = dep->rule->commands;
		while(queue_size(derp) != 0)
			free(queue_dequeue(derp));
		derp = dep->rule->deps;
		while(queue_size(derp) != 0)
			free(queue_dequeue(derp));
		
		free(dep->rule->target);
		dependency_destroy(dep);
		free(dep);
	}
	queue_destroy(&queue);

	free(threads);
	return 0; 
}