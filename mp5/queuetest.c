/** @file queuetest.c
 */

#include <stdio.h>
#include <stdlib.h>

#include "libpriqueue/libpriqueue.h"

int compare1(const void * a, const void * b)
{
	return ( *(int*)a - *(int*)b );
}

int compare2(const void * a, const void * b)
{
	return ( *(int*)b - *(int*)a );
}

int main()
{
	priqueue_t q, q2;

	priqueue_init(&q, compare1);
	priqueue_init(&q2, compare2);

	/* Pupulate some data... */
	int *values = (int*)malloc(100 * sizeof(int));

	int i;
	for (i = 0; i < 100; i++)
		values[i] = i;

	/* Add 5 values, 3 unique. */
	priqueue_offer(&q, &values[12]);
	priqueue_offer(&q, &values[13]);
	priqueue_offer(&q, &values[14]);
	priqueue_offer(&q, &values[12]);
	priqueue_offer(&q, &values[12]);

	printf("printing queue:\n");
	priqueue_print(&q);
	printf("-----\n");

	printf("Total elements: %d (expected 5).\n", priqueue_size(&q));

	int val = *((int *)priqueue_poll(&q));
	printf("Top element: %d (expected 12).\n", val);
	printf("Total elements: %d (expected 4).\n", priqueue_size(&q));

	int val2 = *((int *)priqueue_poll(&q));
	printf("Top element: %d (expected 12).\n", val2);
	printf("Total elements: %d (expected 3).\n", priqueue_size(&q));

    int val3 = *((int *)priqueue_poll(&q));
	printf("Top element: %d (expected 12).\n", val3);
	printf("Total elements: %d (expected 2).\n", priqueue_size(&q));

	priqueue_offer(&q, &values[10]);
	priqueue_offer(&q, &values[30]);
	priqueue_offer(&q, &values[20]);

	priqueue_offer(&q2, &values[10]);
	priqueue_offer(&q2, &values[30]);
	priqueue_offer(&q2, &values[20]);


	printf("Elements in order queue (expected 10 13 14 20 30): ");
	while ( priqueue_size(&q) )
		printf("%d ", *((int *)priqueue_poll(&q)) );
	printf("\n");

          printf("Elements in order queue (expected 30 20 10): ");
          while ( priqueue_size(&q2) )
                  printf("%d ", *((int *)priqueue_poll(&q2)) );
          printf("\n");

	priqueue_destroy(&q2);
	priqueue_destroy(&q);

	free(values);

	return 0;
}
