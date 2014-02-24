/*
 * CS 241
 * The University of Illinois
 */

#ifndef _CONTEST_H_
#define _CONTEST_H_

typedef struct _alloc_stats_t
{
	unsigned long long max_heap_used;
	
	unsigned long memory_uses;
	unsigned long long memory_heap_sum;
} alloc_stats_t;

#endif
