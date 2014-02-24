#include <stdlib.h>
#include <string.h>

typedef struct {
  pthread_mutex_t *mon; /* Should be used whenever modifying the state of philosophser */
  pthread_cond_t **cv; /* Condition variable for each philosopher */
  int *state; /* State of each philosopher: Eating, etc */
  int phil_count; /* # of philosophers */
} phils_t;

phils_t* global_state; 

typedef struct {
  int id;                /* The philosopher's id: 0 to 5 */
  long t0;               /* The time when the program started */
  long ms;               /* The maximum time that philosopher sleeps/eats */
  int *blocktime;        /* Total time that a philosopher is blocked */
  int *blockstarting;    /* If a philsopher is currently blocked, the time that he
                            started blocking */
  int phil_count;
  pthread_mutex_t *blockmon;   /* monitor for blocktime */             
} phil_t;

extern void *initialize_v(int phil_count);
extern void pickup(phil_t *);
extern void putdown(phil_t *);
