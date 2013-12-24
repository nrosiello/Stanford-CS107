/*
 * Functions used to build the hashset/vector combination that indexes word counts
 * within articles.
 */

#ifndef __wordcountindex_
#define __wordcountindex_

typedef struct {
  char *word;
  vector *occ;
} wordSet;

/*
 * Sets up the word count hash.
 * Necessary before adding any word/article pairs.
 */
void InitializeWordCounts(hashset *wordCounts);
#endif
