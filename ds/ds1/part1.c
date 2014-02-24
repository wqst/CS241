#include <stdio.h>
#include <stdlib.h>

/**
 * Compute the length of a string s.
 * @param s a string
 * @return the number of characters preceding the NULL terminator
 */
size_t strlen(const char *s)
{
	size_t len = 0;
	while(*s++) len++;	//scan string until hit null
	return len;
}

/**
 * Lexicographically compare strings s1 and s2.
 * @param s1 a string
 * @param s2 a string
 * @return an integer greater than, equal to, or less than zero if s1 is
 *         greater than, equal to, or less than s2, respectively.
 */
int strcmp(const char *s1, const char *s2)
{
	for(; *s1 && *s2 == *s1; s1++, s2++);
	return *s1 - *s2;
}

/**
 * Copy the string s2 to s1 (including the terminating `\0').
 * @param s1 a mutable string
 * @param s2 a constant string
 * @return a pointer to s1
 */
char *strcpy(char *restrict s1, const char *restrict s2)	//restrict means pointers never overlap, not useful for us
{
	char *p = s1;
	while((*p++ == *s2++));
	return s1;
}

/**
 * Find the first occurence of (char)c in s.
 * The terminating NULL is considered part of the string, 
 * therefore if c is `\0', strchr should locate the 
 * terminating `\0'.
 * @param s a string
 * @param c a character to search for
 * @return a pointer to the located character or NULL if it could not be found
 */
char *strchr(const char *s, int c)
{
	while(*s && *s != c) s++;
	if(*s == '\0' && c != '\0')
		return NULL;
	return (char *)s;	//standard library casts from const to not const
}

