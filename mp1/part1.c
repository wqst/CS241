/** @file part1.c */

/*
 * Machine Problem #1
 * CS 241 Fall2013
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "mp1-functions.h"

/**
 * (Edit this function to print out the ten "Illinois" lines in mp1-functions.c in order.)
 */
int main()
{
	//first
	first_step(81);
	
	//second
	int* i = malloc(sizeof(int));
	*i = 132;
	second_step(i);
	free(i);
	
	//double
	int** j = malloc(sizeof(int*));
	j[0] = malloc(sizeof(int));
	*j[0] = 8942;
	double_step(j);
	free(j[0]);
	free(j);
	
	//strange
	int* k = malloc(sizeof(int));
	int v = k;
	k = 0;
	strange_step(k);
	k = v;	//giving its old address back so we don't lose it
	free(k);
	
	//empty
	char* l = malloc(4 * sizeof(char));
	l[3] = 0;
	empty_step(l);
	free(l);
	
	//two
	char* n = malloc(4 * sizeof(char));
	n[3] = 'u';
	two_step(&n[0], n);
	free(n);
	
	//three
	char* m = malloc(6 * sizeof(char));
	three_step(&m[0], &m[2], &m[4]);
	free(m);
	
	//step step
	char* o = malloc(2 * sizeof(char));
	char* p = malloc(3 * sizeof(char));
	char* q = malloc(4 * sizeof(char));
	o[1] = 0;
	p[2] = 8;
	q[3] = 16;
	step_step_step(o, p, q);
	free(o);
	free(p);
	free(q);
	
	//it_may_be_odd
	char* r = malloc(sizeof(char));
	int s = 5;
	*r = s;
	it_may_be_odd(r, s);
	free(r);
	
	//the_end

	int* t = malloc(sizeof(int));
	*t = 0x20001;
	the_end(t, t);

	free(t);
	
	return 0;
}
