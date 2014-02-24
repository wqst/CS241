/*
 * Machine Problem #1
 * CS 241
 */

#include <stdio.h>
#include "mp1-functions.h"

void first_step(int value)
{
	if (value == 81)
		printf("1: Illinois\n");
	else 
	        printf("1: ERROR\n");
}

void second_step(int *value)
{
	if (*value == 132)
		printf("2: Illinois\n");
	else
                printf("2: ERROR\n");
}

void double_step(int **value)
{
	if (*value[0] == 8942)
		printf("3: Illinois\n");
	else
                printf("3: ERROR\n");
}

void strange_step(int *value)
{
	if (value == 0)
		printf("4: Illinois\n");
	else
                printf("4: ERROR\n");

}

void empty_step(void *value)
{
	/* Sanity check to prevent a seg fault */
	if (value == NULL){
                printf("5: ERROR\n");
		return;
	}

	if (((char *)value)[3] == 0)
		printf("5: Illinois\n");
	else
                printf("5: ERROR\n");
}

void two_step(void *s, char *s2)
{
	/* Sanity check to prevent a seg fault */
	if (s2 == NULL){
                printf("6: ERROR1\n");
		return;
	}
	
	if(s != s2)
		printf("%p != %p\n", s, s2);
	if(s2[3] != 'u')
		printf("%c != 'u'\n", s2[3]);
	
	if (s == s2 && s2[3] == 'u')
		printf("6: Illinois\n");
	else
                printf("6: ERROR2\n");
}

void three_step(char *first, char *second, char *third)
{
	if(third != second + 2)
		printf("%p != %p + 2", third, second);
	if(second != first + 2)
		printf("%p != %p + 2", second, first);
	if (third == second + 2 && second == first + 2)
		printf("7: Illinois\n");
	else
                printf("7: ERROR\n");
}

void step_step_step(char *first, char *second, char *third)
{
	if (third[3] == second[2] + 8 && second[2] == first[1] + 8)
		printf("8: Illinois\n");
	else
                printf("8: ERROR\n");
}

void it_may_be_odd(char *a, int b)
{
	/* Sanity check to prevent a seg fault */
	if (a == NULL){
                printf("9: ERROR\n");
		return;
	}

	if (*a == b && b > 0)
	{
		printf("9: Illinois\n");
	}
	else
                printf("9: ERROR\n");
}

void the_end(void *orange, void *blue)
{
	if(orange == NULL)
		printf("orange == NULL");
	if(orange != blue)
		printf("%p != %p", orange, blue);
	if(((char *)blue)[0] != 1)
		printf("%c != 1", ((char *)blue)[0]);
	if(*((int *)orange) % 3 != 0)
		printf("%i mod 3 != 0", *((int *)orange));

	if (orange != NULL && orange == blue && ((char *)blue)[0] == 1 && *((int *)orange) % 3 == 0)
	{
		printf("10: Illinois\n");
	}
	else
                printf("10: ERROR\n");
}
