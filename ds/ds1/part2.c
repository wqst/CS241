#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "part2.h"

#define INITIAL_SIZE 256

/**
 * Initialize the list.
 * @param list the list to initialize
 */
void list_init(list_t *list)
{
	list->count = 0;
	list->len = INITIAL_SIZE;
	list->buf = malloc(sizeof(int) * INITIAL_SIZE);
}

/**
 * Insert an item into the list.
 * @param list the list to insert an item into
 * @param val the value to insert
 */
void list_insert(list_t *list, int val)
{
	if(list->count >= list->len) {
		list->buf = realloc(list->buf, sizeof(int) * (list->len *= 2));
	}
	list->buf[list->count++] = val;
}

/**
 * Remove all items from a list.
 * @param list the list to clear
 */
void list_clear(list_t *list)
{
	list->count = 0;
}

/**
 * Clone all items from a given list into a new list, removing any items
 * that may be contained in the target in the process.
 * @param source the list to copy
 * @param target the list to copy into
 */
void list_copy(list_t *source, list_t *target)
{
	if(source->count >= target->len) {
		target->buf = realloc(target->buf, source->len * sizeof(int));
		target->len = source->len;
	}

	memcpy(target->buf, source->buf, source->count * sizeof(int));
	target->count = source->count;
}

/**
 * Deallocate any memory associated with a list.
 * @param list the list to free
 */
void list_destroy(list_t *list)
{
	//don't free list because it may be on the stack or something; freeing it would be all kinds of bad
	free(list->buf);
}

/**
 * Compute the average of all items in the list.
 * @param list the list to compute the average of
 * @return the average or nan if the list is empty
 */
double list_average(list_t *list)
{
	if(list->count == 0)
		return 0;
	int total = 0;
	size_t i;
	for(i = 0; i < list->count; i++)
		total += list->buf[i];
	return total / i;
}

/**
 * Find the largest item in the list.
 * @param list the list to search
 * @return the largest value in the input list or INT_MIN if the list is empty
 */
int list_max(list_t *list)
{
	int max = INT_MIN;
	size_t i;
	for(i = 0; i < list->count; i++) {
		if(list->buf[i] > max) max = list->buf[i];
	}
	return max;
}

/**
 * Find the smallest item in the list.
 * @param list the list to search
 * @return the smallest value in the input list or INT_MAX if the list is empty
 */
int list_min(list_t *list)
{
	int min = INT_MAX;
	size_t i;
	for(i = 0; i < list->count; i++) {
		if(list->buf[i] < min) min = list->buf[i];
	}
	return min;
}

/**
 * Check if the list contains a given value.
 * @param list the list to search
 * @param value the value to look for in the list
 * @return 0 if the value is not in the list, 1 if it is
 */
int list_contains(list_t *list, int value)
{
	size_t i;
	for(i = 0; i < list->count; i++) {
		if(list->buf[i] == value) return 1;
	}
	return 0;
}

