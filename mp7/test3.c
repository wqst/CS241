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

void map(int fd, const char *data)
{
	unsigned int i;
	int values[26];
	for (i = 0; i < 26; i++)
		values[i] = 0;

	for (i = 0; i < strlen(data); i++)
	{
		char c = data[i];

		if (isalpha(c))
		{
			c = tolower(c);
			values[c - 'a']++;
		}
	}


	char s[100];
	for (i = 0; i < 26; i++)
	{
		int len = snprintf(s, 100, "%c: %d\n", (i + 'a'), values[i]);
		write(fd, s, len);
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
	mapreduce_t mr;
	mapreduce_init(&mr, map, reduce);

	char *values[5];
	values[0] = "Two roads diverged in a yellow wood,";
	values[1] = "And sorry I could not travel both";
	values[2] = "And be one traveler, long I stood";
	values[3] = "And looked down one as far as I could.";
	values[4] = NULL;

	mapreduce_map_all(&mr, (const char **)values);
	mapreduce_reduce_all(&mr);

	char s[2]; s[1] = '\0';
	int i;
	for (i = 0; i < 26; i++)
	{
		s[0] = (i + 'a');

		const char *s1 = mapreduce_get_value(&mr, s);

		if (s1 == NULL)
			printf("%s: (null)\n", s);
		else
		{
			printf("%s: %s\n", s, s1);
			free((void *)s1);
		}
	}



	mapreduce_destroy(&mr);

	return 0;
}
