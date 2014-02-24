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
	int time = 10 - fd;
	if (time < 0)
		time = 1;

	printf("Sleeping for %d seconds (job #%s)...\n", time, data);
	sleep(time);

	char *s = "value: 1\n";
	write(fd, s, strlen(s));
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
	values[0] = "1";
	values[1] = "2";
	values[2] = "3";
	values[3] = "4";
	values[4] = NULL;

	mapreduce_map_all(&mr, (const char **)values);

	int i;
	for (i = 0; i < 10; i++)
	{
		const char *value = mapreduce_get_value(&mr, "value");
		if (value == NULL) { printf("value: (null)\n"); }
		else { printf("value: %s\n", value); free((void *)value); }
		sleep(1);
	}

	mapreduce_reduce_all(&mr);
	mapreduce_destroy(&mr);

	return 0;
}
