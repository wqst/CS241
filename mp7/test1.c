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
	//printf("length=%ld\n", strlen(data));
	unsigned int i, letters = 0;
	for (i = 0; i < strlen(data); i++)
		if (isalpha(data[i]))
			letters++;

	char s[100];
	sprintf(s, "letters: %d\n", letters);

	//printf("writing %s\n", s);

	write(fd, s, strlen(s));
	close(fd);

	//printf("done writing\n");
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
	char *values[2];
	values[0] = "Some text";
	values[1] = NULL;
	mapreduce_map_all(&mr, (const char **)values);
	mapreduce_reduce_all(&mr);

	const char *s = "letters";
	const char *s1 = mapreduce_get_value(&mr, s);
	if (s1 == NULL)
		printf("%s: (null)\n", s);
	else
	{
		printf("%s: %s\n", s, s1);
		free((void *)s1);
	}


	mapreduce_destroy(&mr);
	return 0;
}
