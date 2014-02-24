/* 
 * CS 241
 * The University of Illinois
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "libmapreduce.h"

#define CHARS_PER_INPUT 30000
#define INPUTS_NEEDED 10

typedef struct _list_t
{
	struct _list_t *next;
	char *word;
	int count;
} list_t;
list_t *head = NULL;

void map(int fd, const char *data)
{
	list_t *thru;

	while (1)
	{
		char *word = malloc(100); int word_len = 0;
		const char *word_end = data;

		/* Get each word... */
		while (1)
		{
			if ( ((*word_end == ' ' || *word_end == '\n') && word_len > 0) || *word_end == '\0' || word_len == 99)
				break;
			else if (isalpha(*word_end))
				word[word_len++] = tolower(*word_end);

			word_end++;
		}
		if (*word_end == '\0') { break; }

		word[word_len] = '\0';

		/* Update our linked list... */
		thru = head;
		while (thru != NULL)
		{
			if ( strcmp(thru->word, word) == 0 )
			{
				thru->count++;
				break;
			}

			thru = thru->next;
		}

		if (thru == NULL)
		{
			list_t *node = malloc(sizeof(list_t));
			node->next = head;
			node->word = word;
			node->count = 1;

			head = node;
		}
		else
			free(word);


		data = word_end + 1;
	}



	thru = head;
	char s[200];
	while (thru != NULL)
	{
		int len = snprintf(s, 200, "%s: %d\n", thru->word, thru->count);
		write(fd, s, len);

		list_t *thru_next = thru->next;
		free(thru->word);
		free(thru);
		thru = thru_next;
	}

	close(fd);
}

const char *reduce(const char *value1, const char *value2)
{
	int i1 = atoi(value1);
	int i2 = atoi(value2);

	char *result;
	asprintf(&result, "%d", (i1 + i2));
	return result;
}


int main()
{
	FILE *file = fopen("alice.txt", "r");
	char s[1024];
	int i;

	char **values = malloc(INPUTS_NEEDED * sizeof(char *));
	int values_cur = 0;
	
	values[0] = malloc(CHARS_PER_INPUT + 1);
	values[0][0] = '\0';

	while (fgets(s, 1024, file) != NULL)
	{
		if (strlen(values[values_cur]) + strlen(s) < CHARS_PER_INPUT)
			strcat(values[values_cur], s);
		else
		{
			values_cur++;
			values[values_cur] = malloc(CHARS_PER_INPUT + 1);
			values[values_cur][0] = '\0';
			strcat(values[values_cur], s);
		}
	}

	values_cur++;
	values[values_cur] = NULL;
	
	fclose(file);




	mapreduce_t mr;
	mapreduce_init(&mr, map, reduce);

	mapreduce_map_all(&mr, (const char **)values);
	mapreduce_reduce_all(&mr);


	const char *result_the = mapreduce_get_value(&mr, "the");
	const char *result_and = mapreduce_get_value(&mr, "and");
	const char *result_alice = mapreduce_get_value(&mr, "alice");
	const char *result_nonexist = mapreduce_get_value(&mr, "some-word-that-wont-exist");

	if (result_the == NULL) { printf("the: (null)\n"); }
	else { printf("the: %s\n", result_the); free((void *)result_the); }

	if (result_and == NULL) { printf("and: (null)\n"); }
	else { printf("and: %s\n", result_and); free((void *)result_and); }

	if (result_alice == NULL) { printf("alice: (null)\n"); }
	else { printf("alice: %s\n", result_alice); free((void *)result_alice); }

	if (result_nonexist == NULL) { printf("some-word-that-wont-exist: (null)\n"); }
	else { printf("some-word-that-wont-exist: %s\n", result_nonexist); free((void *)result_nonexist); }


	mapreduce_destroy(&mr);

	for (i = 0; i < values_cur; i++)
		free(values[i]);
	free(values);


	return 0;
}
