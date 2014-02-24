#include <stdio.h>

typedef struct list list_t;

struct list {
	size_t count;	//# items
	size_t len;		//capacity
	int *buf;		//array
};

void list_init(list_t *list);
void list_insert(list_t *list, int val);
void list_clear(list_t *list);
void list_copy(list_t *source, list_t *target);
void list_destroy(list_t *list);
double list_average(list_t *list);
int list_max(list_t *list);
int list_min(list_t *list);
int list_contains(list_t *list, int value);

