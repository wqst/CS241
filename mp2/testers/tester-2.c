#include <stdio.h>
#include <stdlib.h>

#define M (1024 * 1024)
#define K (1024)

#ifdef PART2
  #define TOTAL_ALLOCS 5*M
#else
  #define TOTAL_ALLOCS 50*K
#endif

int main()
{
	//printf("start");
	malloc(1);

	int i;
	int **arr = malloc(TOTAL_ALLOCS * sizeof(int *));
	if (arr == NULL)
	{
		printf("Memory failed to allocate!\n");
		return 1;
	}

	for (i = 0; i < TOTAL_ALLOCS; i++)
	{
		arr[i] = malloc(sizeof(int));
		//printf("%i has address %p\n", i, arr[i]);
		if (arr[i] == NULL)
		{
			printf("Memory failed to allocate!\n");
			return 1;
		}
	
		*(arr[i]) = i;
	}

	for (i = 0; i < TOTAL_ALLOCS; i++)
	{
		//printf("got %i, should be %i\n", *(arr[i]), i);
		if (*(arr[i]) != i)
		{
			printf("Memory failed to contain correct data after many allocations!\n");
			return 2;
		}
	}

	for (i = 0; i < TOTAL_ALLOCS; i++)
		free(arr[i]);

	free(arr);
	printf("Memory was allocated, used, and freed!\n");	
	return 0;
}
