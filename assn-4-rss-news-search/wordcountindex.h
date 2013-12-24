/*
 * Functions used to build the hashset/vector combination that indexes word counts
 * within articles.
 */

#ifndef __wordcountindex_
#define __wordcountindex_

#include "article.h"

typedef struct {
  char *word;
  vector occ;
} wordSet;

typedef struct {
  article source;
  int count;
} articleCount;

/**
 * Sets up the word count hash.
 * Necessary before adding any word/article pairs.
 */
void InitializeWordCounts(hashset *wordCounts);

/**
 * Enters the specific word/article combination into a word count hashset.  If an
 * entry already exists for this word/article pair, the count is incremented by one.
 * If not, a new entry is created with a count of one.
 */
void WordCountEnter(hashset *wordCount, const char *word, const char *articleTitle, 
    const char *articleURL);

/**
 * Given a populated hashset of word counts, returns a vector of the counts in each
 * article.  Elements of the vector are of type article count and the vector is unsorted.
 */
vector *WordCountLookup(const hashset *wordCount, const char *word);

/**
 * Sorts all the article count vectors to be in descending order for easy querying and
 * printing of the top ten values. Intended to be called once after the word count vector
 * has been built.
 */
void WordCountSort(hashset *wordCount);

#endif
