/** @file ptr_acrobats.c */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/** Structure for each entry in the memory. */
typedef struct entry entry_t;
struct entry {
    void* ptr;
    int size;
};

#define SIZE 10

entry_t entries[SIZE];



/**
 * Populate our initial array using fake memory addresses
 */
void populate()
{
    int i;
    for (i = 0; i < (SIZE / 2); i++) {
        entries[i].ptr = (void*)(0x11111 * i);
        entries[i].size = 10 * i;
    }

    for (i = (SIZE / 2); i < SIZE; i++) {
        entries[i].ptr = (void*)(0x11111 * i);
        entries[i].size = 10 * (SIZE - i);
    }
}



/**
 * Performs a best fit algorithm in the array entries given the
 * parameter size.
 *
 * @param size
 *    The size of the element in which to perform the best
 *    fit algorithm on.
 *
 * @return
 *    The index in which the element that has the best fit in
 *    the array entries.
 *
 *    If none of the elements has that much size, return -1.
 */
int best_fit(int size)
{
	int index = -1;

	int i;
	for(i = 0; i < SIZE; i++) {
		if(entries[i].size >= size && (index == -1 || entries[i].size < entries[index].size))
			index = i;
	}

	return index;
}




/**
 * Performs a best fit algorithm in the array entries given the
 * parameter size.
 *
 * @param size
 *    The size of the element in which to perform the best
 *    fit algorithm on.
 *
 * @return
 *    The pointer to an entry that contains the best fit element
 *    in the array entries.
 *
 *    If none of the elements has that much size, return NULL.
 */
entry_t* best_fit_ptr(int size)
{
    entry_t* entry_it = NULL;
	
	int i = best_fit(size);
	entry_it = i == -1 ? NULL : &entries[i];

    // for (entry_it = entries; ...) 
    
    return entry_it;
}




/**
 * Performs a best fit algorithm in the array entries given the
 * parameter size.
 *
 * @param size
 *    The size of the element in which to perform the best
 *    fit algorithm on.
 *
 * @return
 *    The pointer to an entry that contains the best fit element
 *    in the array entries. Note that unlike the previous function,
 *    this is a void* pointer.
 *
 *    If none of the elements has that much size, return NULL.
 */
void* best_fit_ptr2(int size)
{
    //void* entry_it = NULL;

    // for (entry_it = entries; ...) 

    return best_fit_ptr(size);
}

int main()
{
    int index;

    populate();

    index = best_fit(20);
    printf("index: %d\n", index);

    //   make a call to best_fit_ptr(50) and print out the pointer address 
    //   and entry size using the returned pointer

	entry_t *e = best_fit_ptr(50);
	printf("%p, %i\n", e->ptr, e->size);

    //   make a call to best_fit_ptr2(40) and print out the pointer address 
    //   and entry size using the returned pointer

	e = best_fit_ptr2(40);
	printf("%p, %i\n", e->ptr, e->size);

    return 0;
}
