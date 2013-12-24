#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Case-insensitive string hash function.
 */
static const signed long kHashMultiplier = -1664117991L;
int StringHash(const char *s, int numBuckets)  
{            
  int i;
  unsigned long hashcode = 0;
       
  for (i = 0; i < strlen(s); i++)  
    hashcode = hashcode * kHashMultiplier + tolower(s[i]);  
          
  return hashcode % numBuckets;
}

int wordHashFn(const void *elemAddr, int numBuckets)
{
  char *word = * (char **) elemAddr;
  return StringHash(word, numBuckets);
}

int wordCmpFn(const void *elem1, const void *elem2)
{
  return strcasecmp(*(const char **) elem1, *(const char **) elem2);
}

void wordFreeFn(void *elem)
{
  free(* (void **) elem);
}
