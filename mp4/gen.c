/** @file gen.c
  * CS 241 Fall 2013
  * The University of Illinois
  *
  * Generates a specified number of random, non-negative integers within a range.
  * By default, the integers will be generated within the range [0, RAND_MAX].
  * The integers generated may not be unique.
  *
  * The generated integers will be written to stdout, one per line, and an
  * empty line will terminating the output.
  *
  * Command line options:
  *   --min: Specifies the minimum, inclusive number in the range.
  *          (Default value: 0)
  *   --max: Specifies the maximum, inclusive number in the range.
  *          (Default value: RAND_MAX)
  *   --seed: Specifies a random seed.
  *
  * Usage:
  *   ./gen [--min #] [--max #] [--seed #] <count>
  *   ./gen 100
  *   ./gen -min 0 -max 100 50
  */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>

void print_usage(char *program_name)
{
	fprintf(stderr, "Generates a specified number of random, non-negative integers within a range.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: %s [--min #] [--max #] [--seed #] <count>\n", program_name);
}

int main(int argc, char*argv[])
{
	int count = 0;
	int min = 0;
	int max = RAND_MAX;
	int seed = -1;

	/* Parse command line options */
	while (1)
	{
		static struct option long_options[] =
		{
			{"min", required_argument, 0, 'm'},
			{"max", required_argument, 0, 'n'},
			{"seed", required_argument, 0, 's'},
			{0, 0, 0, 0}
		};

		int option_index = 0;
		int c = getopt_long_only(argc, argv, "m:n:", long_options, &option_index);

		if (c == -1)
			break;

		switch (c)
		{
			case 'm': /* min */
				min = atoi(optarg);
				break;

			case 'n': /* max */
				max = atoi(optarg);
				break;

			case 's': /* seed */
				seed = atoi(optarg);
				break;

			default:
				fprintf(stderr, "error: bad command line option\n");
				print_usage(argv[0]);
		}
	}

    if (optind == argc - 1)
		count = atoi(argv[optind]);
	else
	{
		fprintf(stderr, "error: count not specified\n");
		print_usage(argv[0]);
	}


	/* Parameter checking */
	if (count <= 0)
	{
		fprintf(stderr, "error: count <= 0\n");
		print_usage(argv[0]);
	}

	if (max < min)
	{
		fprintf(stderr, "error: max < min\n");
		print_usage(argv[0]);
	}

	if (min < 0)
	{
		fprintf(stderr, "error: min < 0\n");
		print_usage(argv[0]);
	}

	if (max > RAND_MAX)
	{
		fprintf(stderr, "error: max > RAND_MAX\n");
		print_usage(argv[0]);
	}


	/* Seed rand() */
	if (seed == -1)
		srand(time(NULL));
	else
		srand(seed);

	
	/* Generate */
	int range = max - min + 1;

	int i;
	for (i = 0; i < count; i++)
	{
		int val = (rand() % range) + min;
		printf("%d\n", val);
	}

	exit(EXIT_SUCCESS);
}
