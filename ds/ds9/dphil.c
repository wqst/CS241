#include <stdio.h>
#include <pthread.h>
#include "dphil.h"

enum phil_state { THINKING, HUNGRY, EATING };

/**
 * Check whether the n-th philosopher is hungry. If both of his
 * neighbors are not eating, then he could start eating.
 */
void test(phils_t *pp, int n)
{
  int phil_count;
    
  phil_count = pp->phil_count;

  if (pp->state[(n+(phil_count-1))%phil_count] != EATING &&
      pp->state[n] == HUNGRY &&
      pp->state[(n+1)%phil_count] != EATING) {
    pp->state[n] = EATING;
    pthread_cond_signal(pp->cv[n]);
  }
}

/**
 * Called when the philosopher wants to eat. Keep trying to pickup
 * the chopsticks. When it leaves this method, the philosopher
 * must be eating.
 */
void pickup(phil_t *ps)
{
    /* TODO */
    int n = ps->id;
    int count = global_state->phil_count;
    int left = (n + 1) % count;
    int right = (n + count - 1) % count;

    pthread_mutex_lock(global_state->mon);

    global_state->state[n] = HUNGRY;    //would like to start eating

    while(global_state->state[left] == EATING || global_state->state[right] == EATING) {
        pthread_cond_wait(global_state->cv[n], global_state->mon);  //automatically releases all locks, sleeps until awoken, then reacquires lock
    }

    global_state->state[n] = EATING;

    pthread_mutex_unlock(global_state->mon);
}

/**
 * Called when philosopher is done eating. Set its state back
 * to THINKING. Wake up and check whether the philosopher's left
 * and right neighbor are currently hungry.
 */
void putdown(phil_t *ps)
{
    /* TODO */
    int n = ps->id;
    int count = global_state->phil_count;
    int left = (n + 1) % count;
    int right = (n + count - 1) % count;

    pthread_mutex_lock(global_state->mon);

    global_state->state[n] = THINKING;

    pthread_cond_signal(global_state->cv[left]);    //notify left neighbor
    pthread_cond_signal(global_state->cv[right]);   //notify right neighbor

    pthread_mutex_unlock(global_state->mon);
}

void *initialize_v(int phil_count)
{
  phils_t *pp;
  int i;

  pp = (phils_t *) malloc(sizeof(phils_t));
  pp->phil_count = phil_count;
  pp->mon = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pp->cv = (pthread_cond_t **) malloc(sizeof(pthread_cond_t *)*phil_count);
  if (pp->cv == NULL) { perror("malloc"); exit(1); }
  pp->state = (int *) malloc(sizeof(int)*phil_count);
  if (pp->state == NULL) { perror("malloc"); exit(1); }
  pthread_mutex_init(pp->mon, NULL);
  for (i = 0; i < phil_count; i++) {
    pp->cv[i] = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    if (pp->cv[i] == NULL) { perror("malloc"); exit(1); }
    pthread_cond_init(pp->cv[i], NULL);
    pp->state[i] = THINKING;
  }

  return (void *) pp;
}
