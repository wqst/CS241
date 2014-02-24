#include <stdio.h>
#include <stdlib.h>

extern size_t strlen(char *);
extern int strcmp(const char *, const char *);
extern char *strcpy(char *restrict, const char *restrict);
extern char *strcat(char *restrict, const char *restrict);
extern char *strchr(const char *, int);
extern char *strrchr(const char *, int);
extern char *strsep(char **, const char *);
extern char *strpbrk(const char *, const char *);

int main(int argc, const char *argv[])
{
  int fail;
  char *b, *c;

  printf("CS 241: Discussion Section 1\n");
  printf("Thursday, September 4th, 2013\n");
  printf("Unit tests for string functions.\n");
  printf("----------------------------------------------------------------------------\n");

  // ** strlen() **************************************************************************
  // Test strlen with empty string
  if ( strlen("") == 0 ) printf("strlen(\"\"): pass\n");
  else                   printf("strlen(\"\"): fail\n");

  // Test strlen with non-empty string
  if ( strlen("Hello, World!") == 13 ) printf("strlen(\"Hello, World!\"): pass\n");
  else                                 printf("strlen(\"Hello, World!\"): fail\n");

  // ** strcmp() **************************************************************************
  // Test strcmp with two empty strings
  if ( strcmp("", "") == 0 ) printf("strcmp(\"\", \"\"): pass\n");
  else                       printf("strcmp(\"\", \"\"): fail\n");

  // Test strcmp with one empty string
  if ( strcmp("abc", "") > 0 ) printf("strcmp(\"abc\", \"\"): pass\n");
  else                         printf("strcmp(\"abc\", \"\"): fail\n");

  // Test strcmp with one empty strings
  if ( strcmp("", "abc") < 0 ) printf("strcmp(\"\", \"abc\"): pass\n");
  else                         printf("strcmp(\"\", \"abc\"): fail\n");

  // Test strcmp with two equal strings
  if ( strcmp("abc", "abc") == 0 ) printf("strcmp(\"abc\", \"abc\"): pass\n");
  else                             printf("strcmp(\"abc\", \"abc\"): fail\n");

  // Test strcmp with unequal strings
  if ( strcmp("abc", "abd") < 0 ) printf("strcmp(\"abc\", \"abd\"): pass\n");
  else                            printf("strcmp(\"abc\", \"abd\"): fail\n");

  // Test strcmp with unequal strings
  if ( strcmp("abc", "ab") > 0 ) printf("strcmp(\"abc\", \"ab\"): pass\n");
  else                           printf("strcmp(\"abc\", \"ab\"): fail\n");

  // ** strcpy() **************************************************************************
  //               H     E     L     L     O     \0    0xaa  0xaa
  char buf1[8] = { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa };
  fail=0;

  // Test strcpy with empty string
  if ( strcpy(buf1, "") != buf1 ) { printf("strcpy(buf1, \"\") did not return buf1\n"); fail=1; }
  if ( buf1[0] != '\0' ) { printf("strcpy(buf1, \"\") did not terminate buf1\n"); fail=1; }
  if ( (unsigned char)buf1[1] != 0xaa ) { printf("strcpy(buf1, \"\") did not stop at terminator\n"); fail=1; }

  if ( !fail ) printf("strcpy(buf1, \"\"): pass\n");
  fail=0;
  buf1[0] = '\0';

  // Test strcpy with "Hello"
  if ( strcpy(buf1, "Hello") != buf1 ) { printf("strcpy(buf1, \"Hello\") did not return buf1\n"); fail=1; }
  if ( buf1[5] != '\0' ) { printf("strcpy(buf1, \"Hello\") did not terminate buf1\n"); fail=1; }
  if ( (unsigned char)buf1[6] != 0xaa ) { printf("strcpy(buf1, \"Hello\") did not stop at terminator\n"); fail=1; }

  // Verify that the copy worked
  for (b=buf1, c="Hello"; *b && *c; b++, c++) 
    if ( *b != *c ) { printf("strcpy(buf1, \"Hello\") did not copy properly at index %zu\n", (size_t)(b-buf1)); fail=1; }
  if ( *b || *c ) { printf("strcpy(buf1, \"Hello\") string lengths do not agree\n"); fail=1; }

  if ( !fail ) printf("strcpy(buf1, \"Hello\"): pass\n");

  // ** strcat() **************************************************************************
  //               H     E     L     L     O     ,           W     O     R     L     D     !     \0     0xaa  0xaa
  char buf2[16] = { 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa };
  fail=0;

  // Test strcpy with empty source
  if ( strcpy(buf2, "") != buf2 ) { printf("strcat(buf2, \"\") did not return buf2\n"); fail=1; }
  if ( buf2[0] != '\0' ) { printf("strcat(buf2, \"\") overwrote terminator of buf2\n"); fail=1; }
  if ( (unsigned char)buf2[1] != 0xaa ) { printf("strcat(buf2, \"\") did not stop at terminator\n"); fail=1; }

  if ( !fail ) printf("strcat(buf2, \"\"): pass\n");
  fail=0;
  buf2[0] = '\0';

  // Test strcat with empty target
  if ( strcat(buf2, buf1) != buf2 ) { printf("strcat(buf2, buf1) did not return buf2\n"); fail=1; }
  if ( buf2[5] != '\0' ) { printf("strcat(buf2, buf1) did not terminate buf2\n"); fail=1; }
  if ( (unsigned char)buf2[6] != 0xaa ) { printf("strcat(buf2, buf1) did not stop at terminator\n"); fail=1; }

  // Verify that the copy worked
  for (b=buf2, c="Hello"; *b && *c; b++, c++) 
    if ( *b != *c ) { printf("strcat(buf2, buf1) did not copy properly at index %zu\n", (size_t)(b-buf1)); fail=1; }
  if ( *b || *c ) { printf("strcat(buf2, buf1) string lengths do not agree\n"); fail=1; }

  if ( !fail ) printf("strcat(buf2, buf1): pass\n");
  fail=0;

  // Test strcat with nonempty source and target
  if ( strcat(buf2, ", World!") != buf2 ) { printf("strcat(buf2, \", World!\") did not return buf2\n"); fail=1; }
  if ( buf2[13] != '\0' ) { printf("strcat(buf2, \", World!\") overwrote terminator of buf2\n"); fail=1; }
  if ( (unsigned char)buf2[14] != 0xaa ) { printf("strcat(buf2, \", World!\") did not stop at terminator\n"); fail=1; }

  // Verify that the append worked
  for (b=buf2, c="Hello, World!"; *b && *c; b++, c++) 
    if ( *b != *c ) { printf("strcat(buf2, \", World!\") did not copy properly at index %zu\n", (size_t)(b-buf1)); fail=1; }
  if ( *b || *c ) { printf("strcat(buf2, \", World!\") string lengths do not agree\n"); fail=1; }

  if( !fail ) printf("strcat(buf2, \", World!\"): pass\n");

  // ** strchr() **************************************************************************

  char empty[] = "";
  char not_empty[] = "ABC: DEF: GHI";

  // Test empty string with non-terminator
  if ( strchr(empty, 'a') == NULL ) printf("strchr(\"\", 'a'): pass\n");
  else                              printf("strchr(\"\", 'a'): fail\n");

  // Test empty string with terminator
  if ( strchr(empty, '\0') == empty ) printf("strchr(\"\", '\\0'): pass\n");
  else                                printf("strchr(\"\", '\\0'): fail\n");

  // Test nonempty string with non-terminator
  if ( strchr(not_empty, ':') == &not_empty[3] ) printf("strchr(\"ABC: DEF: GHI\", ':'): pass\n");
  else                                           printf("strchr(\"ABC: DEF: GHI\", ':'): fail\n");

  // Test nonempty string with terminator
  if ( strchr(not_empty, '\0') == &not_empty[13] ) printf("strchr(\"ABC: DEF: GHI\", '\\0'): pass\n");
  else                                             printf("strchr(\"ABC: DEF: GHI\", '\\0'): fail\n");

  // ** strrchr() **************************************************************************

  // Test empty string with non-terminator
  if ( strrchr(empty, 'a') == NULL ) printf("strrchr(\"\", 'a'): pass\n");
  else                               printf("strrchr(\"\", 'a'): fail\n");

  // Test empty string with terminator
  if ( strrchr(empty, '\0') == empty ) printf("strrchr(\"\", '\\0'): pass\n");
  else                                 printf("strrchr(\"\", '\\0'): fail\n");

  // Test nonempty string with non-terminator
  if ( strrchr(not_empty, ':') == &not_empty[8] ) printf("strrchr(\"ABC: DEF: GHI\", ':'): pass\n");
  else                                            printf("strrchr(\"ABC: DEF: GHI\", ':'): fail\n");

  // Test nonempty string with terminator
  if ( strrchr(not_empty, '\0') == &not_empty[13] ) printf("strrchr(\"ABC: DEF: GHI\", '\\0'): pass\n");
  else                                              printf("strrchr(\"ABC: DEF: GHI\", '\\0'): fail\n");

  char *str = "first part:second part;third part";
  printf("Running optional strpbrk() on string %s\n", str);
  printf("strpbrk(\"%s\", \":;\") = \"%s\"\n", str, strpbrk(str, ":;"));

  char tokens[] = "this;is,a,string;broken,by;commas,and,semicolons";
  char *s, *t = tokens;
  printf("Running optional strsep() on string %s\n", t);
  while ( (s = strsep(&t, ",;")) ) printf("%s\n", s);
  return 0;
}

