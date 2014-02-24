/*
 * CS560: Systems
 * 	modified from CS460 Operating Systems -- Jim Plank
 * dphil_skeleton.c -- Dining philosophers driver program
 */


#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "dphil.h"


void *philosopher(void *v)
{
  phil_t *ps;
  long st;
  long t;

  ps = (phil_t *) v;

  while(1) {

    /* First the philosopher thinks for a random number of seconds */

    st = (random()%ps->ms) + 1;
    printf("%3ld Philosopher %d thinking for %ld second%s\n", 
                time(0)-ps->t0, ps->id, st, (st == 1) ? "" : "s");
    fflush(stdout);
    sleep(st);

    /* Now, the philosopher wakes up and wants to eat.  He calls pickup
       to pick up the chopsticks */

    printf("%3ld Philosopher %d no longer thinking -- calling pickup()\n", 
            time(0)-ps->t0, ps->id);
    fflush(stdout);
    t = time(0);
    pthread_mutex_lock(ps->blockmon);
    ps->blockstarting[ps->id] = t;
    pthread_mutex_unlock(ps->blockmon);

    pickup(ps);

    pthread_mutex_lock(ps->blockmon);
    ps->blocktime[ps->id] += (time(0) - t);
    ps->blockstarting[ps->id] = -1;
    pthread_mutex_unlock(ps->blockmon);

    /* When pickup returns, the philosopher can eat for a random number of
       seconds */

    st = (random()%ps->ms) + 1;
    printf("%3ld Philosopher %d eating for %ld second%s\n", 
                time(0)-ps->t0, ps->id, st, (st == 1) ? "" : "s");
    fflush(stdout);
    sleep(st);

    /* Finally, the philosopher is done eating, and calls putdown to
       put down the chopsticks */

    printf("%3ld Philosopher %d no longer eating -- calling putdown()\n", 
            time(0)-ps->t0, ps->id);
    fflush(stdout);
    putdown(ps);
  }
}

int main(int argc, char **argv)
{
  int i;
  pthread_t *threads;
  phil_t *ps;
  long t0, ttmp, ttmp2;
  pthread_mutex_t *blockmon;
  int *blocktime;
  int *blockstarting;
  char s[500];
  int phil_count;
  char *curr;
  int total;

  if (argc != 2) {
    fprintf(stderr, "usage: dphil philosopher_count\n");
    exit(1);
  }

  srandom(time(0));
  
  phil_count = atoi(argv[1]);
  threads = (pthread_t *) malloc(sizeof(pthread_t)*phil_count);
  if (threads == NULL) { perror("malloc"); exit(1); }
  ps = (phil_t *) malloc(sizeof(phil_t)*phil_count);
  if (ps == NULL) { perror("malloc"); exit(1); }
  
   
  global_state = initialize_v(phil_count);
  t0 = time(0);
  blocktime = (int *) malloc(sizeof(int)*phil_count);
  if (blocktime == NULL) { perror("malloc blocktime"); exit(1); }
  blockstarting = (int *) malloc(sizeof(int)*phil_count);
  if (blockstarting == NULL) { perror("malloc blockstarting"); exit(1); }

  blockmon = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(blockmon, NULL);
  for (i = 0; i < phil_count; i++) {
    blocktime[i] = 0;
    blockstarting[i] = -1;
  }

  for (i = 0; i < phil_count; i++) {
    ps[i].id = i;
    ps[i].t0 = t0;
    ps[i].ms = 1;
    ps[i].blocktime = blocktime;
    ps[i].blockstarting = blockstarting;
    ps[i].blockmon = blockmon;
    ps[i].phil_count = phil_count;
    pthread_create(threads+i, NULL, philosopher, (void *) (ps+i));
  }

  while(1) {
    pthread_mutex_lock(blockmon);
    ttmp = time(0);
    curr = s;
    total = 0;
    for(i=0; i < phil_count; i++) {
      total += blocktime[i];
      if (blockstarting[i] != -1) total += (ttmp - blockstarting[i]);
    }
    sprintf(curr,"%3ld Total blocktime: %5d : ", ttmp-t0, total);

    curr = s + strlen(s);

    for(i=0; i < phil_count; i++) {
      ttmp2 = blocktime[i];
      if (blockstarting[i] != -1) ttmp2 += (ttmp - blockstarting[i]);
      sprintf(curr, "%5ld ", ttmp2);
      curr = s + strlen(s);
    }
    pthread_mutex_unlock(blockmon);
    printf("%s\n", s);
    fflush(stdout);
    sleep(10);
  }
}
