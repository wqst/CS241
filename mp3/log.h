/** @file log.h */

#ifndef __LOG_H_
#define __LOG_H_

typedef struct _log_t_entry {
	char* msg;
	struct _log_t_entry* next;
} log_t_entry;

typedef struct _log_t {
	struct _log_t_entry* first;
} log_t;

void log_init(log_t *l);
void log_destroy(log_t* l);
void log_push(log_t* l, const char *item);
char *log_search(log_t* l, const char *prefix);
char *log_search_recursive(log_t_entry* l, const char *prefix);
void log_print(log_t* l);
void log_print_recursive(log_t_entry* l);

#endif
