#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "wordcountindex.h"
#include "hashset.h"
#include "vector.h"
#include "stringhash.h"

static void wordSetFreeFn(void *elemAddr)
{
  wordSet *ws = (wordSet *) elemAddr;
  free(ws->word);
  VectorDispose(&ws->occ);
}

static void articleCountFreeFn(void *elemAddr)
{
  articleCount *ac = (articleCount *) elemAddr;
  free(ac->source.url);
  free(ac->source.title);
}

void InitializeWordCounts(hashset *wordCount)
{
  HashSetNew(wordCount, sizeof(wordSet), 10007, wordHashFn, wordCmpFn, wordSetFreeFn);
}

int articleCountCompareFn(const void *elemAddr1, const void *elemAddr2)
{
  articleCount *ac1 = (articleCount *) elemAddr1;
  articleCount *ac2 = (articleCount *) elemAddr2;
  return articleCompareFn(&ac1->source, &ac2->source);
}

void WordCountEnter(hashset *wordCount, const char *word, const char *articleTitle, 
    const char *articleURL)
{
  /**
   * Three possible cases:
   * 1. Word has not been entered
   * 2. Word has been entered, but word/article combination has not
   * 3. Word/article combination has been entered
   */

  wordSet *existingWord = (wordSet *) HashSetLookup(wordCount, &word);
 
  // ensure that the word exists in the hashset 
  if (existingWord == NULL) {
    wordSet ws;
    ws.word = strdup(word);
    VectorNew(&ws.occ, sizeof(articleCount), articleCountFreeFn, 25); 
    HashSetEnter(wordCount, &ws); 
  }

  // an entry for the word should always exist now
  existingWord = (wordSet *) HashSetLookup(wordCount, &word);
  assert(existingWord != NULL); 

  // now either add the article to the word count vector or increment its current count
  articleCount articleKey = { { (char *) articleTitle, (char *) articleURL }, 1 };
  int existingArticleIndex = VectorSearch(&existingWord->occ, 
     &articleKey, articleCountCompareFn, 0, false); 
  if (existingArticleIndex == -1) {
    // word/article pairing is new, append it to the vector with a count of 1
    articleCount newArticle;
    (newArticle.source).title = strdup(articleTitle);
    (newArticle.source).url = strdup(articleURL);
    newArticle.count = 1;
    VectorAppend(&existingWord->occ, &newArticle);
  } else {
    // word/article pairing exists, increment its count
    articleCount *existingArticle = (articleCount *) VectorNth(&existingWord->occ, existingArticleIndex);
    existingArticle->count++;
  }
}

vector *WordCountLookup(const hashset *wordCount, const char *word)
{
  wordSet *existingWord = (wordSet *) HashSetLookup((hashset *) wordCount, &word);
  if (existingWord == NULL)
    return NULL;
  else
    return &existingWord->occ;
}

int WordCountSortCompareFn(const void *elemAddr1, const void *elemAddr2)
{
  articleCount *ac1 = (articleCount *) elemAddr1;
  articleCount *ac2 = (articleCount *) elemAddr2;
  return ac2->count - ac1->count;
}

void WordCountSortMapFn(void *elemAddr, void *auxData)
{
  wordSet *ws = (wordSet *) elemAddr;
  VectorSort(&ws->occ, WordCountSortCompareFn); 
}

void WordCountSort(hashset *wordCount)
{
  HashSetMap(wordCount, WordCountSortMapFn, NULL);
}
