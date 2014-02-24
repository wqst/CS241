#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>

extern char *strdup(const char *c);
extern char *strtok(char *restrict str, const char *restrict sep);
extern char *strtok_r(char *restrict str, const char *restrict sep, char **restrict lasts);

/**
 * @define NUMBER_OF_TOKENS
 * @brief The number of tokens in test strings.
 *
 * Each test string will be initialized to contain this many tokens separated by spaces.
 */
#define NUMBER_OF_TOKENS     10000000

/**
 * @define NUMBER_OF_ITERATIONS
 * @brief The number of times to retokenize the strings.
 *
 * Each thread will tokenize the test strings this many times to test for consistency.
 */
#define NUMBER_OF_ITERATIONS 5

/**
 * @brief Thread specific information about the tokenization process.
 *
 * Structure contains information used by threads to test strtok() and strtok_r().  On exit 
 * threads will indicate the total number of tokens encountered and the number of mismatches 
 * seen using this structure.
 */
typedef struct {
  /**
   * @var ntimes
   * @brief The number of times to repeat the tokenization process.
   *
   * Each thread will run the tokenization routine (either strtok or strtok_r) this many 
   * times and check the consistency of each token.
   */
  int ntimes;

  /**
   * @var nerrors
   * @brief The number of incorrect tokens encountered.
   *
   * Each time an unexpected token is encountered this counter is incremented to indicate 
   * that the tokenization has failed.
   */
  int nerrors;

  /**
   * @var ntokens
   * @brief Total number of tokens encountered by thread.
   *
   * Each time a thread encounters a token it will increment this value to indicate that it has 
   * read a new token.  This will include all tokens encountered across all iterations.
   */
  int ntokens;

  /**
   * @var string_to_tok
   * @brief The test string to tokenize.
   *
   * The thread will tokenize this string ntimes many times recording the number of tokens 
   * encountered and the number of errors detected.  The string will not be modified by the 
   * thread.
   */
  char *string_to_tok;

  /**
   * @var delim
   * @brief The delimiter string to pass to strtok() or strtok_r().
   *
   * A c-string in which each character may serve as a separator between tokens in 
   * string_to_tok.  This should generally just be a space.  This string will not 
   * be modified.
   */
  char *delim;

  /**
   * @var expect
   * @brief The expected token.
   *
   * Threads will verify that strtok() or strtok_r() is working be using strcmp() to compare 
   * the value of the tokens returned with this string (equality being success).  The string 
   * will not be modified by the thread.
   */
  char *expect;
} arg_t;

/**
 * @brief A thread routine to test strtok().
 *
 * Runs strtok() a series of times on a sample input stream in parallel with other threads to 
 * verify that the output is reasonable.
 *
 * @param arg an arg_t structure describing the test and to contain the results.
 *
 * @returns always returns NULL.
 */
void *thread(void *arg);

/**
 * @brief A thread routine to test strtok_r().
 *
 * Runs strtok_r() a series of times on a sample input stream in parallel with other threads to 
 * verify that the output is reasonable.
 *
 * @param arg an arg_t structure describing the test and to contain the results.
 *
 * @returns always returns NULL.
 */
void *safe_thread(void *arg);

/**
 * @brief The main routine for testing.
 *
 * Spawns two threads to test strtok() in parallel and prints the results to standard output.  Then 
 * spawns two threads to test strtok_r() in parallel and again prints the results to standard output.  
 * This program accepts no arguments.
 */
int main(int argc, const char *argv[])
{
  int i;
  char *s1, *s2, *append1, *append2;
  arg_t thread1, thread2;
  pthread_t tid1, tid2;
  
  thread1.delim = " ";
  thread2.delim = " ";

  thread1.expect = "Hello";
  thread2.expect = "Goodbye";
  
  thread1.ntimes = NUMBER_OF_ITERATIONS;
  thread2.ntimes = NUMBER_OF_ITERATIONS;

  thread1.nerrors = 0;
  thread2.nerrors = 0;

  thread1.ntokens = 0;
  thread2.ntokens = 0;

  thread1.string_to_tok = malloc((strlen(thread1.expect)+1)*NUMBER_OF_TOKENS);
  thread2.string_to_tok = malloc((strlen(thread2.expect)+1)*NUMBER_OF_TOKENS);
  
  s1 = thread1.string_to_tok;
  s2 = thread2.string_to_tok;
  
  *s1 = '\0';
  *s2 = '\0';

  append1 = "Hello ";
  append2 = "Goodbye ";

  for (i=0; i<NUMBER_OF_TOKENS-1; i++) {
    strcat(s1, append1);
    strcat(s2, append2);

    s1 += 6;
    s2 += 8;
  }

  strcat(s1, "Hello");
  strcat(s2, "Goodbye");

  printf("Tokenizing string with %d copies of `%s` in thread1 and %d copies of `%s` in thread2.\n",
      NUMBER_OF_TOKENS, thread1.expect, NUMBER_OF_TOKENS, thread2.expect);

  printf("Tokenizing with strtok()\n");

  pthread_create(&tid1, NULL, thread, &thread1);
  pthread_create(&tid2, NULL, thread, &thread2);

  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);

  printf("Tokenized %d copies of `%s` and encountered %d tokens matching `%s` over %d trials.\n", 
      thread1.ntokens, thread1.expect, thread1.nerrors, thread2.expect, thread1.ntimes);

  printf("Tokenized %d copies of `%s` and encountered %d tokens matching `%s` over %d trials.\n", 
      thread2.ntokens, thread2.expect, thread2.nerrors, thread1.expect, thread2.ntimes);

  thread1.nerrors = 0;
  thread2.nerrors = 0;

  thread1.ntokens = 0;
  thread2.ntokens = 0;

  printf("Tokenizing with strtok_r()\n");

  pthread_create(&tid1, NULL, safe_thread, &thread1);
  pthread_create(&tid2, NULL, safe_thread, &thread2);

  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);

  printf("Tokenized %d copies of `%s` and encountered %d tokens matching `%s` over %d trials.\n", 
      thread1.ntokens, thread1.expect, thread1.nerrors, thread2.expect, thread1.ntimes);

  printf("Tokenized %d copies of `%s` and encountered %d tokens matching `%s` over %d trials.\n", 
      thread2.ntokens, thread2.expect, thread2.nerrors, thread1.expect, thread2.ntimes);

  return 0;
}

void *thread(void *arg)
{
  arg_t *info = (arg_t *)arg;

  int i;
  char *copy, *token;

  for (i=0; i<info->ntimes; i++) {
    copy = strdup(info->string_to_tok);

    token = strtok(copy, info->delim);
    for (;token; token = strtok(NULL, info->delim), info->ntokens++)
      if (strcmp(token, info->expect))
        info->nerrors++;

    free(copy);
  }

  return NULL;
}

void *safe_thread(void *arg)
{
  arg_t *info = (arg_t *)arg;

  int i;
  char *copy, *token, *pos;

  for (i=0; i<info->ntimes; i++) {
    copy = strdup(info->string_to_tok);

    token = strtok_r(copy, info->delim, &pos);
    for (;token; token = strtok_r(NULL, info->delim, &pos), info->ntokens++)
      if (strcmp(token, info->expect))
        info->nerrors++;

    free(copy);
  }

  return NULL;
}

