#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "article.h"
#include "stringhash.h"

int articleHashFn(const void *elemAddr, int numBuckets)
{
  article *a = (article *) elemAddr;
  return StringHash(a->url, numBuckets);  
}

int articleCompareFn(const void *elemAddr1, const void *elemAddr2)
{
  article *a1 = (article *) elemAddr1;
  article *a2 = (article *) elemAddr2;
  return strcasecmp(a1->url, a2->url);
}

void articleFreeFn(void *elemAddr) 
{
  article *a = (article *) elemAddr;
  free(a->url);
  free(a->title);
}

void InitializePrevSeenArticles(hashset *prevSeenArticles)
{
  HashSetNew(prevSeenArticles, sizeof(article), 1009, articleHashFn, 
    articleCompareFn, articleFreeFn);
}

bool IsNewArticle(hashset *prevSeenArticles, char *url, char *title)
{
  article a;
  a.url = url;
  a.title = title;

  void *prevSeen = HashSetLookup(prevSeenArticles, &a);
  if (prevSeen != NULL) {
    return false;
  } else {
    a.url = strdup(url);
    a.title = strdup(title);
    HashSetEnter(prevSeenArticles, &a);
    return true;
  }
} 
