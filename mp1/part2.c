/*
 * Machine Problem #1
 * CS 241
 */

/*
 * You SHOULD modify this file and add more test cases!  The autograder
 * will run many more test cases than the 8 provided below.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "libdictionary/libdictionary.h"

int main()
{
	/*
	 * Initialize the dictionary data structure.
	 */
	dictionary_t dictionary;
	dictionary_init(&dictionary);


	/*
	 * Preform some basic actions
	 */
	int result;
	const char *s;

	/* _add() */
	result = dictionary_add(&dictionary, "key", "value");
	if (result != 0) { printf("_add() failed, and it should have been successful.\n"); }
	else { printf("_add(): OKAY!\n"); }

	result = dictionary_add(&dictionary, "key2", "value");
	if (result != 0) { printf("_add() failed, and it should have been successful.\n"); }
	else { printf("_add(): OKAY!\n"); }

	result = dictionary_add(&dictionary, "key3", "value");
	if (result != 0) { printf("_add() failed, and it should have been successful.\n"); }
	else { printf("_add(): OKAY!\n"); }

	result = dictionary_add(&dictionary, "key", "value2");
	if (result == 0) { printf("_add() was successful, and it should've failed.\n"); }
	else { printf("_add(): OKAY!\n"); }

	/* _remove() */
	dictionary_remove(&dictionary, "key3");

	/* _get() */
	s = dictionary_get(&dictionary, "non-existant");
	if (s != NULL) { printf("_get() was successful, and it should've failed.\n"); }
	else { printf("_get(): OKAY!\n"); }

	s = dictionary_get(&dictionary, "key");
	if (s == NULL || strcmp(s, "value") != 0) { printf("_get() failed or was not the expected result.\n"); }
	else { printf("_get(): OKAY!\n"); }

	/* _parse() */
	char *s1 = malloc(100);
	strcpy(s1, "key3: value");
	result = dictionary_parse(&dictionary, s1);
	if (result != 0) { printf("_parse() failed, and it should have been successful.\n"); }
	else { printf("_parse(): OKAY!\n"); }

	char *s2 = malloc(100);
	strcpy(s2, "bad key-value");
	result = dictionary_parse(&dictionary, s2);
	if (result == 0) { printf("_parse() was successful, and it should've failed.\n"); }
	else { printf("_parse(): OKAY!\n"); }

	/* _get() */
	s = dictionary_get(&dictionary, "key3");
	if (s == NULL || strcmp(s, "value") != 0) { printf("_get() failed or was not the expected result.\n"); }
	else { printf("_get(): OKAY!\n"); }

	//printf("got %s\n", dictionary_get(&dictionary, "key3"));

	/*
	 * Free up the memory used by the dictionary and close the file.
	 */
	dictionary_destroy(&dictionary);
	free(s1);
	free(s2);

	return 0;
}

