#include <stdlib.h>
#include <string.h>
#include <strings.h>

/**
 * @var cur
 * @brief A pointer used to store the next unreturned token by strtok()
 *
 * This variable is used to remember the position of the next unreturned token in a string being processed 
 * by strtok().  Once the last token has been returned it is set to NULL to indicate tokenization is complete.
 */
static volatile char * volatile cur = NULL;

/**
 * @brief Tokenize an input string iteratively.
 *
 * Initially strtok() is called with a string (str) that is to be tokenized and a list of separaters (sep)
 * passed as a null terminated c-string in which each character is a valid delimiter.  Subsequent calls to 
 * strtok() should be made with a NULL first parameter to indicate that the same string is being tokenized.
 * Each call to strtok() will return the next token in the string (as a NULL termiated c-string) until the 
 * last token has been returned, after which strtok() will return NULL to indicate the end of the string 
 * has been reached.  Each call to strtok() will insert a NULL terminator at the position of the original 
 * input string containing the next delimiter.
 *
 * This function is NOT thread-safe.
 *
 * @param str a new string to tokenize, or NULL if tokenization is to continue on a previously indicated string.
 * @param sep a c-string in which each character can serve as a delimiter between tokens.
 *
 * @returns a pointer to the beginning of the next token as a c-string, or NULL if the last token has been reached.
 */
char *strtok(char *restrict str, const char *restrict sep)
{
  volatile char *vs = str;

  if (!vs)
    vs = cur;  // get the value of cur if we are continuing from an old string

  if (!vs)
    return NULL;        // return NULL if tokenization is complete

  volatile char *last = vs;

  while (!strchr(sep, *vs)) vs++; // Search for the next token
  if (*vs) *vs++ = '\0';          // Create a substring for the next token...
  else      vs   = NULL;          // ...or indicate we have reached the last token

  cur = vs;    // update the global state
  return (char *)last;
}

/**
 * @brief Tokenize an input string iteratively in a thread safe fashion.
 *
 * Initially strtok_r() is called with a string (str) that is to be tokenized and a list of separaters (sep)
 * passed as a null terminated c-string in which each character is a valid delimiter.  Subsequent calls to 
 * strtok_r() should be made with a NULL first parameter to indicate that the same string is being tokenized.
 * Each call to strtok_r() will return the next token in the string (as a NULL termiated c-string) until the 
 * last token has been returned, after which strtok_r() will return NULL to indicate the end of the string 
 * has been reached.  Each call to strtok_r() will insert a NULL terminator at the position of the original 
 * input string containing the next delimiter.  Each call to strtok_r() should include a pointer to a character 
 * pointer.  After each call the pointer will be updated to point at the start of the next token not yet returned.
 *
 * This function IS thread-safe.
 *
 * @param str   a new string to tokenize, or NULL if tokenization is to continue on a previously indicated string.
 * @param sep   a c-string in which each character can serve as a delimiter between tokens.
 * @param lasts a character pointer to use as a buffer to remember the next unreturned token between calls.
 *
 * @returns a pointer to the beginning of the next token as a c-string, or NULL if the last token has been reached.
 */
char *strtok_r(char *restrict str, const char *restrict sep, char **restrict lasts)
{
  volatile char *vs = str;

  if (!vs)
    vs = *lasts;  // get the value of cur if we are continuing from an old string

  if (!vs)
    return NULL;        // return NULL if tokenization is complete

  volatile char *last = vs;

  while (!strchr(sep, *vs)) vs++; // Search for the next token
  if (*vs) *vs++ = '\0';          // Create a substring for the next token...
  else      vs   = NULL;          // ...or indicate we have reached the last token

  *lasts = (char*)vs;    // update the global state
  return (char *)last;
  return NULL;
}

