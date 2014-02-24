/*
 * CS 241
 * The University of Illinois
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/types.h>
#include "contest.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>

static void *alloc_handle = NULL;

static void *(*alloc_calloc)(size_t nmemb, size_t size) = NULL;
static void *(*alloc_malloc)(size_t size) = NULL;
static void  (*alloc_free)(void *ptr) = NULL;
static void *(*alloc_realloc)(void *ptr, size_t size) = NULL;

static void *(*libc_calloc)(size_t nmemb, size_t size) = NULL;
static void *(*libc_malloc)(size_t size) = NULL;
static void (*libc_free)(void *ptr) = NULL;
static void *(*libc_realloc)(void *ptr, size_t size) = NULL;

static void *sbrk_start = 0;
static void *sbrk_largest = 0;
static void *sbrk_init_done = 0;

static alloc_stats_t *stats = NULL;

static int inside_init = 0;


static void contest_alloc_init()
{
	inside_init = 1;
	
	/* Tell malloc() not to use mmap() */
	mallopt(M_MMAP_MAX, 0);
	
	sbrk_start = sbrk_largest = sbrk(0);
	
	libc_calloc  = dlsym(RTLD_NEXT, "calloc");
	libc_malloc  = dlsym(RTLD_NEXT, "malloc");
	libc_free    = dlsym(RTLD_NEXT, "free");
	libc_realloc = dlsym(RTLD_NEXT, "realloc");
	
	inside_init = 2;
	
	alloc_handle = dlopen("./alloc.so", RTLD_NOW | RTLD_GLOBAL);
	if (!alloc_handle)
	{
		char *err =  dlerror();

		if (!err)
			fprintf(stderr, "A dynamic linking error occurred: (%s)\n", err);
		else
			fprintf(stderr, "An unknown dynamic linking error occurred.\n");

		exit(65);
	}

	alloc_calloc  = dlsym(alloc_handle, "calloc");
	alloc_malloc  = dlsym(alloc_handle, "malloc");
	alloc_free    = dlsym(alloc_handle, "free");
	alloc_realloc = dlsym(alloc_handle, "realloc");

	if (!alloc_calloc || !alloc_malloc || !alloc_free || !alloc_realloc)
	{
		fprintf(stderr, "Unable to dynamicly load a required memory allocation call.\n");
		exit(66);
	}
	
	char *file_name = getenv("ALLOC_CONTEST_MMAP");
	int fd = open(file_name, O_RDWR);
	stats = mmap(NULL, sizeof(alloc_stats_t), PROT_WRITE, MAP_SHARED, fd, 0);

	if (fd <= 0 || stats == (void *)-1)
	{
		fprintf(stderr, "fd/mmap");
		exit(67);
	}
	
	stats->max_heap_used = 0;
	stats->memory_heap_sum = 0;
	stats->memory_uses = 0;
	
	sbrk_init_done = sbrk(0);
	inside_init = 0;
}

static void contest_tracking()
{
	void *sbrk_current = sbrk(0);
	unsigned long current_mem_usage = ((long)sbrk_current - (long)sbrk_init_done);
	
	if (stats->max_heap_used < current_mem_usage)
	{
		sbrk_largest = sbrk_current;
		stats->max_heap_used = current_mem_usage;

		if (current_mem_usage > (1024L * 1024L * 1024L * 2L))
		{
			fprintf(stderr, "Exceeded 2 GB\n");
			exit(68);
		}
	}
	
	stats->memory_heap_sum += current_mem_usage;
	stats->memory_uses++;
}

void *calloc(size_t nmemb, size_t size)
{
	if (inside_init)
	{
		if (inside_init == 1)
		{
			void *ptr = sbrk(nmemb * size);
			memset(ptr, 0x00, nmemb * size);
			return ptr;
		}
		else
			return libc_calloc(nmemb, size);
	}
		
	if (!alloc_handle)
		contest_alloc_init();

	void *addr = alloc_calloc(nmemb, size);
	contest_tracking();

	return addr;
}

void *malloc(size_t size)
{
	if (inside_init)
		return libc_malloc(size);
	
	if (!alloc_handle)
		contest_alloc_init();

	void *addr = alloc_malloc(size);
	contest_tracking();

	return addr;
}

void free(void *ptr)
{
	if (inside_init)
	{
		libc_free(ptr);
		return;
	}
	
	if (!alloc_handle)
		contest_alloc_init();
	
	if (ptr < sbrk_init_done)
	{
		libc_free(ptr);
		return;
	}

	if (ptr)
	{
		alloc_free(ptr);
		contest_tracking();
	}
}

void *realloc(void *ptr, size_t size)
{
	if (inside_init)
		return libc_realloc(ptr, size);
	
	if (!alloc_handle)
		contest_alloc_init();

	void *addr;
	if (!ptr)
		addr = alloc_malloc(size);
	else if (size == 0)
	{	
		alloc_free(ptr);
		addr = NULL;
	}
	else
		addr = alloc_realloc(ptr, size);
	contest_tracking();

	return addr;
}

