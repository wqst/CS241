/** @file queue.c */

#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
/**
 * Private.  Linked-list node
 *
 */
struct queue_node {
	void *item; ///<Stored value
	struct queue_node *next; ///<Link to next node
};
 
/**
 * Initializes queue structure.
 * Should always be called first.
 *
 * @param q A pointer to the queue data structure.
 * @return void
 */
void queue_init(queue_t *q) {

	q->head = NULL;
	q->tail = NULL;
	q->size = 0;
}

/**
 * Frees all associated memory.
 * Should always be called last.
 *
 * @param q A pointer to the queue data structure.
 * @return void
 */
void queue_destroy(queue_t *q) {
	while(queue_size(q) > 0) {
		queue_dequeue(q);
	}
}

/**
 * Removes and returns element from front of queue.
 * 
 * @param q A pointer to the queue data structure.
 * @return A pointer to the oldest element in the queue.
 * @return NULL if the queue is empty.
 */
void *queue_dequeue(queue_t *q) {
	struct queue_node *front;
	void *item;

	if(queue_size(q) == 0) {
		return NULL;
	}

	front = q->head;
	q->head = q->head->next;
	q->size--;

	item = front->item;
	free(front);

	if(queue_size(q) == 0) {
		// just cleaning up
		q->head = NULL;
		q->tail = NULL;
	}

	return item;
}


/**
 * Removes and returns element at position pos.
 *
 * @param q A pointer to the queue data structure.
 * @param pos Position to be removed.
 * @return A pointer to the element at position pos.
 * @return NULL if the position is invalid.
 */
void *queue_remove_at(queue_t *q, int pos){
	if( pos < 0 || q->size-1 < (unsigned int)pos)
		return NULL;
	q->size--;
	struct queue_node *cur = q->head;
	struct queue_node *prev = NULL;
	while(pos--){
		prev = cur;
		cur = cur->next;
	}

	if(cur == q->head){
		q->head = cur->next;
		if(q->head == NULL)
			q->tail = NULL;
		void *item = cur->item;
		free(cur);
		return item;
	}else if(cur == q->tail){
		q->tail = prev;
		prev->next = NULL;
		void *item = cur->item;
		free(cur);
		return item;
	}else{
		prev->next = cur->next;
		void *item = cur->item;
		free(cur);
		return item;
	}
}

/**
 * Returns element located at position pos.
 *
 * @param q A pointer to the queue data structure.
 * @param pos Zero-based index of element to return.
 * @return A pointer to the element at position pos.
 * @return NULL if position out of bounds.
 */
void *queue_at(queue_t *q, int pos){
	int i;
	struct queue_node *node;
	if(q == NULL)
		return NULL;
		

	for(i=0, node=q->head; i<pos && node != NULL; i++) node=node->next;
	if(i != pos)
		return 0;
	else
		return node->item;

}

/**
 * Stores item at the back of the queue.
 *
 * @param q A pointer to the queue data structure.
 * @param item Value of item to be stored.
 * @return void
 */
void queue_enqueue(queue_t *q, void *item) {
	struct queue_node *back = malloc(sizeof(struct queue_node));

	back->item = item;
	back->next = NULL;
	if(queue_size(q) == 0) {
		q->head = back;
	} else {
		q->tail->next = back;
	}
	q->tail = back;
	q->size++;
}

/**
 * Returns number of items in the queue.
 *
 * @param q A pointer to the queue data structure.
 * @return The number of items in the queue.
 */
unsigned int queue_size(queue_t *q) {
	return q->size;
}

/**
 * Helper function to apply operation on each item.
 *
 * @param q A pointer to the queue data structure.
 * @param iter_func Function pointer to operation to be applied.
 * @param arg Pass through variable to iter_func.
 * @return void
 */
void queue_iterate(queue_t *q, void (*iter_func)(void *, void *), void *arg) {
	struct queue_node *node;
	if(queue_size(q) == 0) {
		return;
	}

	node = q->head;
	while(node != NULL) {
		iter_func(node->item, arg);
		node = node->next;
	}
}
