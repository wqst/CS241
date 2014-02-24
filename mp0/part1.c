#include <stdio.h>
#include <stdlib.h>

/**
 * Calculates a value (c) based on the input parameters (a, b) and prints
 * out the result.
 *
 * @param a
 *     Input parameter a.
 *
 * @param b
 *     Input parameter b.
 */
void one(const int a, const int b)
{
	int c = (a * a) + (b * b);
	//cout << a << "^2 + " << b << "^2 = " << c << endl;
	printf("%d^2 + %d^2 = %d\n", a, b, c);
}


/**
 * Checks if the input parameter is a passing grade and prints out if
 * the grade is passing.
 *
 * @param grade
 *     The grade to check.
 */
void two(const int grade)
{
	if(grade > 70)
		printf("You passed!\n");
}


/**
 * Assigns a pointer (int *p) the value of a stack variable (int x).
 */
void three()
{
	int x = 4;
	//int *p = x;
	int *p = malloc(sizeof(int));
	*p = x;

	printf("The value of p is: %d\n", *p);
	free(p);
}


/**
 * Prints out a specific message based on if the number is
 * between 0 and 1 (exclusive).
 *
 * @param value
 *     Value to test.
 */
void four(const float value)
{
	if (0 < value && value < 1)
		printf("The value is between zero and one.\n");
	else
		printf("The value is not between zero and one.\n");
}


/**
 * Prints out a specific message based on if the two input parameters
 * are equal.
 *
 * @param x
 *     First input parameter.
 *
 * @param y
 *     Second input parameter.
 */
void five(const int x, const int y)
{
	if (x == y)
		printf("x and y are equal.\n");
	else
		printf("x and y are different.\n");
}







