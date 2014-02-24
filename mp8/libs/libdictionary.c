/* 
 * CS 241
 * The University of Illinois
 */
 
/** @file libdictionary.c*/
#define _GNU_SOURCE
#include <search.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "libdictionary.h"

/** Internal use only. */
static int compare(const void *a, const void *b)
{
	return strcmp(((const dictionary_entry_t *)a)->key, ((const dictionary_entry_t *)b)->key); 
}

/** Internal use only. */
static dictionary_entry_t *malloc_entry_t(const char *key, const char *value)
{
	dictionary_entry_t *entry = malloc(sizeof(dictionary_entry_t));
	entry->key = key;
	entry->value = value;

	return entry;
}

/** Internal use only. */
static dictionary_entry_t *dictionary_tfind(dictionary_t *d, const char *key)
{
	dictionary_entry_t tentry = {key, NULL};
	void *tresult = tfind((void *)&tentry, &d->root, compare);

	if (tresult == NULL)
		return NULL;
	else
		return *((dictionary_entry_t **)tresult);
}

/** Internal use only. */
static void dictionary_tdelete(dictionary_t *d, const char *key)
{
	dictionary_entry_t tentry = {key, NULL};
	tdelete((void *)&tentry, &d->root, compare);
}

/** Internal use only. */
static int dictionary_remove_options(dictionary_t *d, const char *key, int free_memory)
{
	dictionary_entry_t *entry = dictionary_tfind(d, key);

	if (entry == NULL)
		return NO_KEY_EXISTS;
	else
	{
		dictionary_tdelete(d, key);

		if (free_memory)
		{
			free((void *)entry->key);
			free((void *)entry->value);
		}
		free(entry);

		return 0;
	}
}

/** Internal use only. */
static void destroy_no_element_free(void *ptr)
{
	free(ptr);
}

/** Internal use only. */
static void destroy_element_free(void *ptr)
{
	free((void *)((dictionary_entry_t *)ptr)->key);
	free((void *)((dictionary_entry_t *)ptr)->value);
	free(ptr);
}

/**
 * Must be called first, initializes the  
 * dictionary data structure. Same as MP1.
 */
void dictionary_init(dictionary_t *d)
{
	d->root = NULL;
	pthread_mutex_init(&d->mutex, NULL);
}


/**
 * Adds a (key, value) pair to the dictionary. 
 * @return 0 on success or KEY_EXISTS if    
 * the key already exists in the dictionary.
 */
int dictionary_add(dictionary_t *d, const char *key, const char *value)
{
	pthread_mutex_lock(&d->mutex);

	if (dictionary_tfind(d, key) == NULL)
	{
		tsearch((void *)malloc_entry_t(key, value), &d->root, compare);

		pthread_mutex_unlock(&d->mutex);
		return 0;
	}
	else
	{
		pthread_mutex_unlock(&d->mutex);
		return KEY_EXISTS;
	}
}


/**
 * Retrieves the value from the dictionary for a given key.
 *
 * @return The stored value associated with the key 
 * if the key exists in the dictionary. If the key does not exist, 
 * this function will return NULL.
 */
const char *dictionary_get(dictionary_t *d, const char *key)
{
	pthread_mutex_lock(&d->mutex);
	dictionary_entry_t *entry = dictionary_tfind(d, key);

	if (entry == NULL)
	{
		pthread_mutex_unlock(&d->mutex);
		return NULL;
	}
	else
	{
		pthread_mutex_unlock(&d->mutex);
		return entry->value;
	}
}


/**
 * Parses the key_value string and add the parsed key and value to the dictionary.
 * If successful, the key_value string will be modified in-place in order to be
 * added to the dictionary.  
 *
 * This function only accepts key_value strings in the general format of "Key: Value".
 *
 * @return On success, the return value is zero.  If the key already exists in
 *         the dictionary, KEY_EXISTS is returned; if the format of key_value
 *         is illegal, ILLEGAL_FORMAL is returned.
 */
int dictionary_parse(dictionary_t *d, char *key_value)
{
	char *delim;
	if ((delim = strstr(key_value, ": ")) == NULL)
		return ILLEGAL_FORMAT;
	*delim = '\0';

	int result;
	if ((result = dictionary_add(d, key_value, delim + 2)) != 0)
		*delim = ':';

	return result;
}


/**
 * Removes the (key, value) entry from the dictionary. 
 * @return 0 on success or NO_KEY_EXISTS if the key was not  
 * present in the dictionary. This function does not free the memory used by key or value.
 */
int dictionary_remove(dictionary_t *d, const char *key)
{
	pthread_mutex_lock(&d->mutex);
	int val = dictionary_remove_options(d, key, 0);
	pthread_mutex_unlock(&d->mutex);

	return val;
}


/**
 * Frees all internal memory associated with the dictionary. Must be called last.
 */
void dictionary_destroy(dictionary_t *d)
{
	tdestroy(d->root, destroy_no_element_free);
	d->root = NULL;

	pthread_mutex_destroy(&d->mutex);
}

/**
 * Frees all internal memory associated with the dictionary and
 * key/values pairs. Must be called last.
 */
void dictionary_destroy_all(dictionary_t *d)
{
	tdestroy(d->root, destroy_element_free);
	d->root = NULL;

	pthread_mutex_destroy(&d->mutex);
}

