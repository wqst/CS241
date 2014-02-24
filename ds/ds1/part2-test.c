#include <stdio.h>
#include <stdlib.h>

#ifndef SOL
  #include "part2.h"
#else
  #include "part2-sol.h"
#endif

int main(int argc, const char *argv[])
{
  list_t list1, list2;

  list_init(&list1);
  list_init(&list2);

  int i;
  for (i = 0; i < 512; i++) {
    list_insert(&list1, i);
  }

  list_copy(&list1, &list2);

  printf("Average (list1) = %f\n", list_average(&list1));
  printf("Min (list1) = %i\n", list_min(&list1));
  printf("Max (list1) = %i\n", list_max(&list1));

  list_clear(&list1);
  printf("Clearing list1\n");

  printf("Average (list2) = %f\n", list_average(&list2));
  printf("Min (list2) = %i\n", list_min(&list2));
  printf("Max (list2) = %i\n", list_max(&list2));

  printf("Average (list1) = %f\n", list_average(&list1));
  printf("Min (list1) = %i\n", list_min(&list1));
  printf("Max (list1) = %i\n", list_max(&list1));

  list_destroy(&list1);
  list_destroy(&list2);

  printf("Testing empty list operations\n");

  list_init(&list1);
  list_init(&list2);

  printf("Average (list1) = %f\n", list_average(&list1));
  printf("Min (list1) = %i\n", list_min(&list1));
  printf("Max (list1) = %i\n", list_max(&list1));

  list_clear(&list1);
  list_copy(&list1, &list2);

  list_destroy(&list1);
  list_destroy(&list2);

  printf("Testing destroy empty list\n");
  list_init(&list1);
  list_destroy(&list1);

  return 0;
}

