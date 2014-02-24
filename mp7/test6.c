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

static const char *long_key = "long_key";

/* Takes input string and maps the longest
 * word to the key, long_key.
 */
void map(int fd, const char *data)
{
	//strtok modifies the data so we can't use it here i think
	char* longest = NULL;		//current longest word
	int longest_count = -1;		//length of current longest word
	int last_word_start = 0;	//position of the start of the current word
	int end = strlen(data);
	int i;
	for(i = 0; i < end; i++) {
		if(!isalpha(data[i])) {
			//end of word, check length from last_word_start to i, realloc and overwrite longest and change longest_count
			int length = i - last_word_start;
			if(length > longest_count) {
				longest = realloc(longest, sizeof(char) * (1 + length));
				longest_count = length;
				strncpy(longest, data + last_word_start, length);
			}
			last_word_start = i + 1;
		}
	}
	
	char out[longest_count + 8];
	sprintf(out, "long_key: %s\n", longest);
	write(fd, out, strlen(out));
	close(fd);
}

/* Takes two words and reduces to the longer of the two
 */
const char *reduce(const char *value1, const char *value2)
{
	return strlen(value1) > strlen(value2) ? value1 : value2;		//ternary op
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
	
	const char *result_longest = mapreduce_get_value(&mr, long_key);

	if (result_longest == NULL) { printf("MapReduce returned (null).  The longest word was not found.\n"); }
	else { printf("Longest Word: %s\n", result_longest); free((void *)result_longest); }

	mapreduce_destroy(&mr);

	for (i = 0; i < values_cur; i++)
		free(values[i]);
	free(values);

	return 0;
}
