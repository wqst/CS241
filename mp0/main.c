#include <stdio.h>
#include <stdlib.h>
#include "main.h"

int main()
{
	/* part1.c  */
	printf("== one() ==\n");
	one(3, 4);
	one(10, 10);


	printf("== two() ==\n");
	two(50);
	two(100);


	printf("== three() ==\n");
	three();


	printf("== four() ==\n");
	four(0.5);
	four(1.5);


	printf("== five() ==\n");
	five(3, 3);
	five(3, 4);


	/* part2.c */
	printf("== six() ==\n");
	float *p_six;
	int i4 = 4, i432 = 432;

	p_six = six(&i4);
	printf("%d == %f\n", i4, *p_six);
	free(p_six);

	p_six = six(&i432);
	printf("%d == %f\n", i432, *p_six);
	free(p_six);


	printf("== seven() ==\n");
	seven(2, 12);
	seven(14, 20);


	printf("== eight() ==\n");
	eight();


	printf("== nine() ==\n");
	nine();


	printf("== ten() ==\n");
	int i_ten = 100;
	ten(&i_ten);
	printf("%d == 0?\n", i_ten);


	/* part3.c */
	printf("== eleven() ==\n");
	eleven();


	printf("== twelve() ==\n");
	twelve();


	printf("== thirteen() ==\n");
	thirteen();


	printf("== fourteen() ==\n");
	fourteen("red");
	fourteen("orange");
	fourteen("blue");
	fourteen("green");


	printf("== fifteen() ==\n");
	fifteen(1);
	fifteen(2);
	fifteen(3);


	/* part4.c */
	printf("== sixteen() ==\n");
	char *str = sixteen();
	printf("%s\n", str);
	free(str);


	printf("== seventeen() ==\n");
	seventeen(35);
	seventeen(20);


	printf("== eighteen() ==\n");
	eighteen(3);
	eighteen(5);


	printf("== clear_bits() ==\n");
	long int result;
	
	result = clear_bits(0xFF, 0x55);
	printf("%ld\n", result);

	result = clear_bits(0x00, 0xF0);
	printf("%ld\n", result);

	result = clear_bits(0xAB, 0x00);
	printf("%ld\n", result);

	result = clear_bits(0xCA, 0xFE);
	printf("%ld\n", result);

	result = clear_bits(0x14, 0x00);
	printf("%ld\n", result);

	result = clear_bits(0xBB, 0xBB);
	printf("%ld\n", result);


	return 0;
}
