#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARG 64
#define MAX_CWD_LEN 256

const char *prompt = "$(pid=%i) ";

void simple_shell(){
	ssize_t line_ct;
	size_t line_size = 0;
	char *line = NULL;
	pid_t pid = getpid();

	for(;;) {
		printf(prompt, pid);
		fflush(stdout);
		
		line_ct = getline(&line, &line_size, stdin);
		if(line_ct <= 0) {
			printf("\n");
			break;
		}

		line[--line_ct] = '\0';	//remove trailing newline

		if(!strcmp(line, "exit")) {
			break;
		}

		if(!strncmp(line, "cd ", 3)) {	//strncmp compares first n characters of the two strings
			if(chdir(line + 3) == -1) {
				printf("%s: path not found\n", line + 3);
			}
			continue;
		}

		//fork(); -> execvp(argv[0], argv)
		system(line);
	}
	free(line);
}

void fork_chain(int n){
	int i;
	for(i = 0; i < n; i++) {
		if(fork() != 0)
			break;
	}
	pid_t pid = getpid();	//this process's pid
	pid_t ppid = getppid();	//this process's parent's pid
	wait(NULL);
	printf("pid = %i (parent = %i)\n", pid, ppid);
}

void fork_fan(int n){
	int i;
	for(i = 0; i < n; i++) {
		if(fork() == 0)
			break;
		wait(NULL);
	}
	pid_t pid = getpid();	//this process's pid
	pid_t ppid = getppid();	//this process's parent's pid
	printf("pid = %i (parent = %i)\n", pid, ppid);
}

int main()
{
    simple_shell();

    pid_t parent = getpid();

    printf("fork_chain(5):\n");
    fork_chain(5);

    if(getpid() != parent) return 0;	//parent only after this point

    printf("fork_fan(5):\n");
    fork_fan(5);
	return 0;
}
