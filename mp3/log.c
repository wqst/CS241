/** @file log.c */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "log.h"

/**
 * Initializes the log.
 *
 * You may assume that:
 * - This function will only be called once per instance of log_t.
 * - This function will be the first function called per instance of log_t.
 * - All pointers will be valid, non-NULL pointer.
 *
 * @returns
 *   The initialized log structure.
 */
void log_init(log_t *l) {
	l->first = malloc(sizeof(log_t_entry));
	l->first->next = NULL;
	l->first->msg = NULL;
}

/**
 * Frees all internal memory associated with the log.
 *
 * You may assume that:
 * - This function will be called once per instance of log_t.
 * - This funciton will be the last function called per instance of log_t.
 * - All pointers will be valid, non-NULL pointer.
 *
 * @param l
 *    Pointer to the log data structure to be destoryed.
 */
void log_destroy(log_t* l) {
	log_t_entry* target = l->first;
	log_t_entry* next;
	while(target != NULL) {
		next = target->next;
		free(target->msg);
		free(target);
		target = next;
	}
}

/**
 * Push an item to the log stack.
 *
 *
 * You may assume that:
* - All pointers will be valid, non-NULL pointer.
*
 * @param l
 *    Pointer to the log data structure.
 * @param item
 *    Pointer to a string to be added to the log.
 */
void log_push(log_t* l, const char *item) {
	log_t_entry* entry = malloc(sizeof(log_t_entry));
	entry->next = l->first;
	//entry->msg = malloc((strlen(item) + 1) * sizeof(char));
	//strcpy(entry->msg, item);
	entry->msg = item;
	l->first = entry;
}


/**
 * Preforms a newest-to-oldest search of log entries for an entry matching a
 * given prefix.
 *
 * This search starts with the most recent entry in the log and
 * compares each entry to determine if the query is a prefix of the log entry.
 * Upon reaching a match, a pointer to that element is returned.  If no match
 * is found, a NULL pointer is returned.
 *
 *
 * You may assume that:
 * - All pointers will be valid, non-NULL pointer.
 *
 * @param l
 *    Pointer to the log data structure.
 * @param prefix
 *    The prefix to test each entry in the log for a match.
 *
 * @returns
 *    The newest entry in the log whose string matches the specified prefix.
 *    If no strings has the specified prefix, NULL is returned.
 */
char *log_search(log_t* l, const char *prefix) {
	return log_search_recursive(l->first, prefix);
}

char *log_search_recursive(log_t_entry* l, const char *prefix) {
	if(l == NULL || l->msg == NULL)
		return NULL;
	if(!strncmp(l->msg, prefix, strlen(prefix)))
		return l->msg;
    return log_search_recursive(l->next, prefix);
}

/**
 * Prints out the log in back-to-front order to stdout.
 */
void log_print(log_t* l) {
	log_print_recursive(l->first);
}

void log_print_recursive(log_t_entry* l) {
	if(l == NULL || l->msg == NULL)
		return;
	log_print_recursive(l->next);
	printf("%s\n", l->msg);
}