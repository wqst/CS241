/** @file shell.c */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "log.h"


log_t* Log;

const char *prompt = "(pid=%d)%s$ ";

int stop = 0;
/*
char** split_string(char* input) {
	char **ap, *argv[10];
	for(ap = argv; (*ap = strsep(&input, " \t")) != NULL;)
		if(**ap != '\0')
			if(++ap >= &argv[10])
				break;
	return argv;
}
*/
/**
 * Starting point for shell.
 */
int main() {
	Log = malloc(sizeof(log_t));
	log_init(Log);
	ssize_t line_ct;
	size_t line_size = 0;
	char* line = NULL;
	pid_t pid = getpid();
	char* cwd = NULL;
	int derp = 0;

	int buf_size = 4096;
	char buf[buf_size];

	while(!stop) {
		cwd = getcwd(buf, buf_size);
		printf(prompt, pid, cwd);
		fflush(stdout);
		line_ct = getline(&line, &line_size, stdin);
		if(line_ct <= 0) {
			printf("\n");
			break;
		}

		if(strlen(line) <= 1)
			continue;

		line[--line_ct] = '\0';

		//printf("got \"%s\"\n", line);

		if(!strcmp(line, "!#")) {
			printf("Command executed by pid=%d\n", getpid());
			log_print(Log);
			continue;
		}

		if(!strncmp(line, "!", 1)) {
			printf("Command executed by pid=%d\n", getpid());
			char* prefix = line + 1;
			char* result = log_search(Log, prefix);
			if(result == NULL) {
				printf("No match\n");
				continue;
			}
			printf("%s matches %s\n", line + 1, result);
			//free(line);
			line = realloc(line, (strlen(result) + 1) * sizeof(char));
			strcpy(line, result);
		}

		log_push(Log, line);

		if(!strcmp(line, "exit")) {
			printf("Command executed by pid=%d\n", getpid());
			stop = 1;
			break;
		}

		if(!strncmp(line, "cd ", 3)) {
			printf("Command executed by pid=%d\n", getpid());
			if(chdir(line + 3) == -1)
				printf("%s: No such file or directory\n", line + 3);
			continue;
		}
		pid_t child_pid = fork();
		int status;

		if(!child_pid) {	//this is the child process
			char** args = NULL;
			char* temp = strtok(line, " ");
			int count = 0;
			while(temp) {
				//printf("%p\n", temp);
				args = realloc(args, (1 + count) * sizeof(char*));
				if(args == NULL)
					exit(-1);
				args[count] = temp;
				count++;
				temp = strtok(NULL, " ");
			}
			args = realloc(args, (1 + count) * sizeof(char*));
			args[count] = NULL;

			if(execvp(args[0], args) == -1)
				printf("%s: not found\n", args[0]);
			
			//printf("all done\n");
			free(args);
			//free(temp);
			stop = 1;
			derp = 0;
		}
		else {
			printf("Command executed by pid=%d\n", child_pid);
			wait(&status);
			derp = 1;
		}

		//if(line != NULL)
			//free(line);
	}
	if(derp == 1) {
		//printf("freeing cwd!\n");
		//free(cwd);
	}
	//printf("ending\n");
	log_destroy(Log);
	free(Log);
	if(line != NULL)
		free(line);
    return 0;
}
