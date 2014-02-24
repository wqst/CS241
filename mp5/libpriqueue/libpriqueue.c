/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


void priqueue_print(priqueue_t *q) {

	int i;
	priqueue_item_t* curr = q->head;
	for(i = 0; i < q->length; i++) {
		printf("item %d: %d\n", i, *((int*)(curr->item)));
		curr = curr->next;
	}
}

/**
  Initializes the priqueue_t data structure.
  
  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
	q->compare = comparer;
	q->length = 0;
	q->head = NULL;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
	priqueue_item_t* add = (priqueue_item_t*)malloc(sizeof(priqueue_item_t));
	add->item = ptr;
	add->next = NULL;
	if(q->head == NULL) {
		q->head = add;
		q->length++;
		return 0;
	}
	int i = 0;
	priqueue_item_t* curr = q->head;
	priqueue_item_t* prev = NULL;
	while(curr != NULL) {
		if(q->compare(curr->item, add->item) > 0) {
			if(prev != NULL)
				prev->next = add;
			add->next = curr;
			q->length++;
			if(i == 0)
				q->head = add;
			return i;
		}
		i++;
		prev = curr;
		curr = curr->next;
	}
	prev->next = add;
	q->length++;
	return q->length - 1;

}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
	if(q->head != NULL)
		return q->head->item;
	return NULL;
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
	if(q->head == NULL)
		return NULL;
	void* out = q->head->item;
	priqueue_item_t* next = q->head->next;
	free(q->head);
	q->length--;
	q->head = next;
	return out;
}


/**
  Returns the number of elements in the queue.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return q->length;
}


/**
  Destroys and frees all the memory associated with q.
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
	priqueue_item_t* curr = q->head;
	priqueue_item_t* next = NULL;
	while(curr != NULL) {
		next = curr->next;
		free(curr);
		curr = next;
	}
}