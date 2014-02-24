/** @file alloc.c */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

typedef struct _metadata {
	size_t size;
	char used;
	struct _metadata* next;
	struct _metadata* prev;

	char mem[];
} metadata;

metadata* free_start = NULL;

//p ((metadata*)X)[-1] should print out a struct in gdb

#define PTR(p) (&((metadata *)(p))[-1])
#define META sizeof(metadata)
#define DBG 0
#define DBG2 0

/**
 * Allocate space for array in memory
 * 
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 * 
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size)
{
	/* Note: This function is complete. You do not need to modify it. */
	void *ptr = malloc(num * size);
	
	if (ptr)
		memset(ptr, 0x00, num * size);

	return ptr;
}


/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size)
{
	//void* end = sbrk(0);
	metadata* derp = free_start;
	metadata* last = NULL;
	
	while(derp != NULL) {
		//printf("-%p-", derp);
		if(DBG) printf("looking at address %p (usable %p), next is %p\n", derp, derp + META, derp->next);
		if(derp->size >= size && !derp->used) {
			if(DBG) printf("found that %p (usable %p) is free and less than current brk %p\n", derp, derp + META, sbrk(0));
			derp->used = 1;
			//derp->size = size;
			
			if(last != NULL)
				last->next = derp->next;
			derp->prev = last;

			if(DBG) printf("requested block of size %ld, gave address %p to %p\n", size, (metadata*)((void*)derp + META), (metadata*)((void*)derp + META + derp->size));
			return derp->mem;
		}
		/*
		if(derp != NULL && (void*)derp + derp->size + META >= end) {
			sbrk(size - derp->size);
			derp->size = size;
			derp->used = 1;
			return derp->mem;
		}
		*/
		last = derp;
		derp = derp->next;
	}
	
	metadata* herp = sbrk(size + META);
	if(DBG) printf("allocating %ld + metadata = %ld bytes from address %p, usable space starts at %p, actual size is %ld\n", size, size + META, herp, herp->mem, (long int)sbrk(0) - (long int)(herp->mem));
	herp->used = 1;
	herp->size = size;
	herp->next = NULL;
	herp->prev = last;
	/*
	if(free_start == NULL)
		free_start = herp;
	*/
	return herp->mem;
}


/**
 * Deallocate space in memory
 * 
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr)
{
	// "If a null pointer is passed as argument, no action occurs."
	if (!ptr)
		return;
	metadata* entry = PTR(ptr);
	if(DBG) printf("freeing %p (usable %p)\n", ptr, entry);
	if(DBG && free_start != NULL) printf("free_start->next is %p\n",((metadata*)free_start)->next);
	if(DBG) printf("free_start is %p, entry->next is %p\n", free_start, entry->next);
	//look for extra space in the next block
	/*
	metadata* next = ptr + entry->size;
	if((void*)next < sbrk(0) && !next->used) {
		if(DBG) printf("coalescing\n");
		entry->size += META + next->size;
		entry->prev = next->prev;
		if(entry->prev != NULL)
			((metadata*)entry->prev)->next = entry;
		entry->next = next->next;
		if(next == free_start) {
			entry->prev = NULL;
			free_start = entry;
		}
		assert(entry != entry->next);
	}
	else {
		if(DBG) printf("not coalescing\n");
		if(entry != free_start)
			entry->next = free_start;
		entry->prev = NULL;
		free_start = entry;
		assert(entry != entry->next);
	}
	*/
	if(entry->prev != NULL) {
		entry->next = entry->prev->next;
		entry->prev->next = entry;
	}
	//entry->next = free_start;
	//entry->prev = NULL;
	//free_start = entry;
	//assert(entry != entry->next);
	entry->used = 0;
	if(free_start == NULL) {
		free_start = entry;
		//printf("it's a free start");	
	}
	//if(DBG) printf("freed block: size is %ld, address is %p (usable %p), prev is %p, next is %p, free_start is %p\n", entry->size, entry, ptr, entry->prev, entry->next, free_start);
	return;
}


/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *    
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size)
{
	 // "In case that ptr is NULL, the function behaves exactly as malloc()"
	if (!ptr)
		return malloc(size);

	 // "In case that the size is 0, the memory previously allocated in ptr
	 //  is deallocated as if a call to free() was made, and a NULL pointer
	 //  is returned."
	if (!size)
	{
		free(ptr);
		return NULL;
	}

	metadata* xyzzy = PTR(ptr);

	if(DBG || DBG2) printf("omg realloc from %ld to %ld\n", ((metadata*)xyzzy)->size, size);

	if(((metadata*)xyzzy)->size >= size) {
		//don't do anything
		return ptr;
	}
	if(((metadata*)xyzzy)->size < size) {
		//copy
		void* dest = malloc(size);
		size_t bytes = ((metadata*)xyzzy)->size;
		if(DBG2 || DBG) printf("memcpying block from %p to %p, copying %ld bytes\n", ptr, dest, bytes);
		memcpy(dest, ptr, bytes);
		free(ptr);
		return dest;
	}
	return ptr;
}
