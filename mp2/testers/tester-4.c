#include <stdio.h>
#include <stdlib.h>

#define K 1024
#define M (1024 * 1024)
#define G (1024 * 1024 * 1024)

#ifdef PART2
  #define START_MALLOC_SIZE (1536 * M)
  #define STOP_MALLOC_SIZE  (1 * K)
#else
  #define START_MALLOC_SIZE (1 * G)
  #define STOP_MALLOC_SIZE  (1 * K)
#endif

void *reduce(void *ptr, int size)
{
	if (size > STOP_MALLOC_SIZE)
	{
		//printf("splitting size %i into %i\n", size, size/2);
		void *ptr1 = realloc(ptr, size / 2);
		void *ptr2 = malloc(size / 2);

		if (ptr1 == NULL || ptr2 == NULL)
		{
			printf("Memory failed to allocate!\n");
			exit(1);
		}

		ptr1 = reduce(ptr1, size / 2);
		ptr2 = reduce(ptr2, size / 2);
		
		//printf("Got %i and %i, should be %i\n", *((int *)ptr1), *((int *)ptr2), size / 2);
		
		if (*((int *)ptr1) != size / 2 || *((int *)ptr2) != size / 2)
		{
			printf("Memory failed to contain correct data after many allocations!\n");
			exit(2);
		}

		free(ptr2);
		ptr1 = realloc(ptr1, size);

		//printf("Got %i, should be %i\n", *((int *)ptr1), size / 2);

		if (*((int *)ptr1) != size / 2)
		{
			printf("Memory failed to contain correct data after realloc()!\n");
			exit(3);
		}

		*((int *)ptr1) = size;
		return ptr1;
	}
	else
	{
		*((int *)ptr) = size;
		return ptr;
	}


}


int main()
{
	malloc(1);
	
	int size = START_MALLOC_SIZE;
	while (size > STOP_MALLOC_SIZE)
	{
		void *ptr = malloc(size);
		ptr = reduce(ptr, size / 2);
		free(ptr);
		
		size /= 2;
	}

	printf("Memory was allocated, used, and freed!\n");	
	return 0;
}
