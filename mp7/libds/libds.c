/** @file libds.c */
/* 
 * CS 241
 * The University of Illinois
 */

#define _GNU_SOURCE
#include <search.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "libds.h"


typedef struct _datastore_entry_t
{
	const char *key, *value;
	unsigned long rev;
} datastore_entry_t;


/** Private. */
static int compare(const void *a, const void *b)
{
	return strcmp(((const datastore_entry_t *)a)->key, ((const datastore_entry_t *)b)->key); 
}

/** Private. */
static datastore_entry_t *malloc_entry_t(const char *key, const char *value)
{
	datastore_entry_t *entry = malloc(sizeof(datastore_entry_t));
	entry->key = key;
	entry->value = value;
	entry->rev = 1;

	return entry;
}

/** Private. */
static datastore_entry_t *dictionary_tfind(datastore_t *ds, const char *key)
{
	datastore_entry_t tentry = {key, NULL, 0};
	void *tresult = tfind((void *)&tentry, &ds->root, compare);

	if (tresult == NULL)
		return NULL;
	else
		return *((datastore_entry_t **)tresult);
}

/** Private. */
static void dictionary_tdelete(datastore_t *ds, const char *key)
{
	datastore_entry_t tentry = {key, NULL, 0};
	tdelete((void *)&tentry, &ds->root, compare);
}

/** Private. */
static void destroy_with_element_free(void *ptr)
{
	datastore_entry_t *entry = (datastore_entry_t *)ptr;

	free((void *)entry->key);
	free((void *)entry->value);
	free(entry);
}


/**
 * Initializes the data store.
 *
 * @param ds
 *    An uninitialized data store.
 */
void datastore_init(datastore_t *ds)
{
	ds->root = NULL;
	pthread_mutex_init(&ds->mutex, NULL);
}


/**
 * Adds the key-value pair (key, value) to the data store, if and only if the
 * key does not already exist in the data store.
 *
 * The data store will make an internal copy key and value, if necessary, so
 * the user of the data store may free these strings if necessary.
 *
 * This function is thread-safe.
 *
 * @param ds
 *   An initialized data store.
 * @param key
 *   The key to be added to the data store.
 * @param value
 *   The value to associated with the new key.
 *
 * @retval 0
 *   The key already exists in the data store.
 * @retval non-zero
 *   The revision number assigned to the specific value for the given key.
 */
unsigned long datastore_put(datastore_t *ds, const char *key, const char *value)
{
	pthread_mutex_lock(&ds->mutex);

	if (dictionary_tfind(ds, key) == NULL)
	{
		datastore_entry_t *entry = malloc_entry_t(strdup(key), strdup(value));
		tsearch((void *)entry, &ds->root, compare);
		pthread_mutex_unlock(&ds->mutex);

		return entry->rev;
	}
	else
	{
		pthread_mutex_unlock(&ds->mutex);

		return 0;
	}
}


/**
 * Retrieves the current value, and its revision number, for a specific key.
 *
 * This function is thread-safe.
 *
 * @param ds
 *   An initialized data store.
 * @param key
 *   The specific key to retrieve the value.
 * @param revision
 *   If non-NULL, the revision number of the returned value will be written
 *   to the address pointed to by <code>revision</code>.
 *
 * @return
 *   If the data store contains the key, a new string containing the value
 *   will be returned.  It is the responsibility of the user of the data
 *   store to free the value returned.  If the data store does not contain
 *   the key, NULL will be returned and <code>revision</code> will be unmodified.
 */
const char *datastore_get(datastore_t *ds, const char *key, unsigned long *revision)
{
	pthread_mutex_lock(&ds->mutex);
	datastore_entry_t *entry = dictionary_tfind(ds, key);

	if (entry == NULL)
	{
		pthread_mutex_unlock(&ds->mutex);
		return NULL;
	}
	else
	{
		if (revision != NULL)
			*revision = entry->rev;
		const char *value = strdup(entry->value);

		pthread_mutex_unlock(&ds->mutex);
		return value;
	}
}


/**
 * Updates the specific key in the data store if and only if the
 * key exists in the data store and the key's revision in the data
 * store matches the knwon_revision specified.
 *
 * The data store will make an internal copy key and value, if necessary, so
 * the user of the data store may free these strings if necessary.
 *
 * This function is thread-safe.
 * 
 * @param ds
 *   An initialized data store.
 * @param key
 *   The specific key to update in the data store.
 * @param value
 *   The updated value to the key in the data store.
 * @param known_revision
 *   The revision number for the key specified that is expected to be found
 *   in the data store.  If the revision number specified in calling the
 *   function does not match the revision number in the data store, this
 *   function will not update the data store.
 *
 * @retval 0
 *    The revision number specified did not match the revision number in the
 *    data store or the key was not found in the data store.  If the key is
 *    in the data store, this indicates that the data has been modified since
 *    you last performed a datastore_get() operation.  You should get an
 *    updated value from the data store.
 * @retval non-zero
 *    The new revision number for the key, now associated with the new value.
 */
unsigned long datastore_update(datastore_t *ds, const char *key, const char *value, unsigned long known_revision)
{
	pthread_mutex_lock(&ds->mutex);
	datastore_entry_t *entry = dictionary_tfind(ds, key);

	if (entry == NULL)
	{
		// key does not exist
		pthread_mutex_unlock(&ds->mutex);
		return 0;
	}
	else
	{
		// ensure revisions match
                if (entry->rev != known_revision) {
		        pthread_mutex_unlock(&ds->mutex);
			return 0;
                }

		free((void *)entry->value);
		entry->value = strdup(value);
		unsigned long new_revision = ++(entry->rev);

		pthread_mutex_unlock(&ds->mutex);
		return new_revision;
	}
}


/**
 * Deletes a specific key from the data store.
 *
 * This function is thread-safe.
 *
 * @param ds
 *   An initialized data store.
 * @param key
 *   The specific key to update in the data store.
 * @param known_revision
 *   The revision number for the key specified that is expected to be found
 *   in the data store.  If the revision number specified in calling the
 *   function does not match the revision number in the data store, this
 *   function will not update the data store.
 *
 * @retval 0
 *    The revision number specified did not match the revision number in the
 *    data store or the key was not found in the data store.  If the key is
 *    in the data store, this indicates that the data has been modified since
 *    you last performed a datastore_get() operation.  You should get an
 *    updated value from the data store.
 * @retval non-zero
 *    The key was deleted from the data store.
 */
unsigned long datastore_delete(datastore_t *ds, const char *key, unsigned long known_revision)
{
	pthread_mutex_lock(&ds->mutex);
	datastore_entry_t *entry = dictionary_tfind(ds, key);

	if (entry == NULL)
	{
		pthread_mutex_unlock(&ds->mutex);
		return 0;
	}
	else
	{
		if (entry->rev != known_revision)
			return 0;

		dictionary_tdelete(ds, key);

		free((void *)entry->key);
		free((void *)entry->value);
		free(entry);

		pthread_mutex_unlock(&ds->mutex);
		return 1;
	}
}


/**
 * Destroys the data store, freeing any memory that is in use by the
 * data store.
 *
 * @param ds
 *   An initialized data store.
 */
void datastore_destroy(datastore_t *ds)
{
	tdestroy(ds->root, destroy_with_element_free);
	ds->root = NULL;

	pthread_mutex_destroy(&ds->mutex);
}
