#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Concatenates the strings "Hello " and "World!" together, and
 * prints the concatenated string.
 */
void eleven()
{
	char *s = "Hello ";
	char *t = "World!";

	char *out = malloc((strlen(s) + strlen(t) + 1) * sizeof(char));

	strcpy(out, s);
	strcat(out, t);
	printf("%s\n", out);
	//free(s);
	//free(t);
	free(out);
}


/**
 * Creates an array of values containing the values {0.0, 0.1, ..., 0.9}.
 */
void twelve()
{
	float *values = malloc(10 * sizeof(float));

	int i, n = 10;
	for (i = 0; i < n; i++)
		values[i] = (float)i / n;

	for (i = 0; i < n; i++)
		printf("%f ", values[i]);
	printf("\n");

	free(values);
}


/**
 * Creates a 2D array of values and prints out the values on the diagonal.
 */
void thirteen()
{
	int **values;

	int i, j;
	values = malloc(10 * sizeof(int *));

	for(i = 0; i < 10; i++) {
		values[i] = malloc(10 * sizeof(int));
		for(j = 0; j < 10; j++) {
			values[i][j] = i * j;
		}
	}

	for (i = 0; i < 10; i++)
		printf("%d ", values[i][i]);
	printf("\n");

	for(i = 0; i < 10; i++)
		free(values[i]);
	free(values);
}


/**
 * Prints out a specific string based on the input parameter (s).
 *
 * @param s
 *     Input parameter, used to determine which string is printed.
 */
void fourteen(const char *s)
{
	if(strcmp(s, "blue") == 0)
		printf("Orange and BLUE!\n");
	else if(strcmp(s, "orange") == 0)
		printf("ORANGE and blue!\n");
	else
		printf("orange and blue!\n");
}


/**
 * Prints out a specific string based on the input parameter (value).
 *
 * @param value
 *     Input parameter, used to determine which string is printed.
 */
void fifteen(const int value)
{
	switch (value)
	{
		case 1:
			printf("You passed in the value of one!\n");
			break;

		case 2:
			printf("You passed in the value of two!\n");
			break;

		default:
			printf("You passed in some other value!\n");
	}
}
