#include <stdio.h>
#include <stdlib.h>

int main()
{
	int *ptr = malloc(sizeof(int));
	
	if (ptr == NULL)
	{
		printf("Memory failed to allocate!\n");
		return 1;
	}

	*ptr = 4;
	free(ptr);

	printf("Memory was allocated, used, and freed!\n");	
	return 0;
}
