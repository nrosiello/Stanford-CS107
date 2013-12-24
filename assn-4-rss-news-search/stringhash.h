/**
 * Case-insensitive string hash.
 */

#ifndef __stringhash_
#define __stringhash_ 

/**
 * Generic case-insensitive string hash.
 */
int StringHash(const char *s, int numBuckets); 

/**
 * Functions intended to be used with the vector and hashset implementations
 * when storing dynamically allocated C strings.
 */
int wordHashFn(const void *elemAddr, int numBuckets);
int wordCmpFn(const void *elem1, const void *elem2);
void wordFreeFn(void *elem);

#endif
