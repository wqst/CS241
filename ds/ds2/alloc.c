/** @file alloc.c */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/** Structure for each dicitonary entry. */
typedef struct _dictionary_entry_t {
    size_t len;
    struct _dictionary_entry_t *next;

    char mem[]; //can only do at the end of the struct, returns address immediately after struct
} dictionary_entry_t;

dictionary_entry_t *dictionary = NULL;

#define PTR_TO_DICT(p) (&((dictionary_entry_t *)(p))[-1])

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
void *calloc(size_t num, size_t size) {
    /* Note: This function is complete. You do not need to modify it. */
    void *ptr = malloc(num * size);
	
    if (ptr) {
        memset(ptr, 0x00, num * size);
    }

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
void *malloc(size_t size) {
    dictionary_entry_t **entry = &dictionary;
    dictionary_entry_t **found = NULL;

    for(; *entry; entry = &(*entry)->next) {
        if((*entry)->len >= size) {
            if(!found || (*entry)->len < (*found)->len) {
                found = entry;
            }
        }
    }

    if(found) {
        void *mem = (*found)->mem;
        *found = (*found)->next;
        return mem;
    }

    dictionary_entry_t *new_mem = sbrk(sizeof(dictionary_entry_t) + size);  //sbrk is expensive, maybe sbrk more than you need and set the extra aside
    new_mem->len = size;
    new_mem->next = NULL;

    return new_mem->mem;    //all pointers cast to void pointers so we can return any pointer here
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
void free(void *ptr) {
    // "If a null pointer is passed as argument, no action occurs."
    if (!ptr)
        return;

    dictionary_entry_t *entry = PTR_TO_DICT(ptr);
    entry->next = dictionary;
    dictionary = entry;
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
void *realloc(void *ptr, size_t size) {
    // "In case that ptr is NULL, the function behaves exactly as malloc()"
    if (!ptr) {
        return malloc(size);
    }

    // "In case that the size is 0, the memory previously allocated in ptr
    //  is deallocated as if a call to free() was made, and a NULL pointer
    //  is returned."
    if (!size) {
        free(ptr);
        return NULL;
    }

    dictionary_entry_t *entry = PTR_TO_DICT(ptr);
    void *new_mem = malloc(size);
    size_t copy_bytes = entry->len < size ? entry->len : size;
    memcpy(new_mem, ptr, copy_bytes);
    free(ptr);

    return new_mem;
}
