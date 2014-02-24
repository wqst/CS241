#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>

void child(int id, int write_fd) {
    sleep(id);
    char s[100];
    sprintf(s, "%d", id);
    write(write_fd, s, strlen(s));
}
    

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: ./ds10 n\n");
        return 0;
    }
    int n = atoi(argv[1]);
    int fd[n][2];

    int efd = epoll_create(n);
    struct epoll_event ev;
    bzero(&ev.data, sizeof(ev.data));

    int i;
    for (i=0; i<n; i++) {
        pipe(fd[i]);
        pid_t pid = fork();
        int read_fd = fd[i][0];
        int write_fd = fd[i][1];
        if (pid == 0) {
            /* close unused pipes */
            /* add your code here */
            int j;
            for(j = 0; j <= i; j++)
                close(fd[j][0]);
            child(i, write_fd);
            close(write_fd);
            exit(0);
        } else {
            /* close unused pipes */
            /* add your code here */
            close(write_fd);
        }
        /* add fd to epoll */
        /* add your code here */
        ev.events = EPOLLIN;
        ev.data.fd = read_fd;
        epoll_ctl(efd, EPOLL_CTL_ADD, read_fd, &ev);
    }
    
    int counter = n;
    while (counter) {
        char s[100];
        /* wait for an available pipe */
        /* add your code here */
        epoll_wait(efd, &ev, 1, -1);


        ssize_t bytes_read = read(ev.data.fd, s, 100);
        if (bytes_read > 0) {
            printf("%s\n", s);
        } else {
            --counter;
            /* delete pipe */
            /* add your code here */
            close(ev.data.fd);
        }
    }

    /* close unused pipes */
    /* add your code here */
    close(efd);
    
    return 0;
}
