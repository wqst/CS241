/** @file libdictioanry.h */

/*
 * Machine Problem #1
 * CS 241 Fall2013
 */

#ifndef LIBDICTIONARY_H__
#define LIBDICTIONARY_H__

extern const int NO_KEY_EXISTS;
extern const int KEY_EXISTS;
extern const int ILLEGAL_FORMAT;

typedef struct _dictionary_entry_t
{
	struct _dictionary_entry_t *next;
	const char* value;
	const char* key;

} dictionary_entry_t;

typedef struct _dictionary_t
{
	/* You may find this dictionary_entry_t pointer useful.
	   You are not required to use it and may remove it
	   but your data structure MUST shrink/expand based on
	   the size of entries. */
	dictionary_entry_t *first;
	dictionary_entry_t *last;
} dictionary_t;


void dictionary_init(dictionary_t *d);
int dictionary_add(dictionary_t *d, const char *key, const char *value);
int dictionary_parse(dictionary_t *d, char *key_value);
const char *dictionary_get(dictionary_t *d, const char *key);
int dictionary_remove(dictionary_t *d, const char *key);
void dictionary_destroy(dictionary_t *d);

#endif
