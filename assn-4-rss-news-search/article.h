/*
 * Functions and struct used to build the hashset of previously seen articles.
 */

#ifndef __article_
#define __article_

#include "hashset.h"

typedef struct {
  char *title;
  char *url;
} article;

/**
 * Article functions designed to be used with the vector and hashset.
 */
int articleHashFn(const void *elemAddr, int numBuckets);
int articleCompareFn(const void *elemAddr1, const void *elemAddr2);
void articleFreeFn(void *elemAddr);

/**
 * This initialization function needs to be called before any articles are added to
 * the hashset of previously seen articles.
 */
void InitializePrevSeenArticles(hashset *prevSeenArticles);

/**
 * Checks to see if the given article is new.  If it is not in the previously seen articles
 * hashset, returns true and adds the article to the hashset.  Else, returns false.
 */
bool IsNewArticle(hashset *prevSeenArticles, char *url, char *title);

#endif
