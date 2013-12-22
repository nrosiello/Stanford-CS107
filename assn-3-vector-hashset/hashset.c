#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int HashSetElemBucket(const hashset *h, const void *elemAddr)
{
  assert(elemAddr != NULL);
  int bucket = h->hashfn(elemAddr, h->numBuckets);
  assert(bucket >= 0);
  assert(bucket < h->numBuckets);
  return bucket;
}

static vector *HashSetElemVector(const hashset *h, const void *elemAddr)
{
  int bucket = HashSetElemBucket(h, elemAddr);
  return h->buckets + bucket;
}

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
  assert(elemSize > 0);
  assert(numBuckets > 0);
  assert(hashfn != NULL);
  assert(comparefn != NULL);

  h->elemSize = elemSize;
  h->numBuckets = numBuckets;
  h->hashfn = hashfn;
  h->comparefn = comparefn;
  h->freefn = freefn;

  h->buckets = malloc(sizeof(vector) * numBuckets);
  for (int i = 0; i < numBuckets; i++) {
    VectorNew(h->buckets + i, elemSize, freefn, 10);
  }    
}

void HashSetDispose(hashset *h)
{}

int HashSetCount(const hashset *h)
{ return 0; }

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
  for (int i = 0; i < h->numBuckets; i++) 
    VectorMap(h->buckets + i, mapfn, auxData);
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
  void *existing = HashSetLookup(h, elemAddr);
  if (existing) {
    memcpy(existing, elemAddr, h->elemSize);
  } else {
    vector *v = HashSetElemVector(h, elemAddr);
    VectorAppend(v, elemAddr);
  }
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{ 
  vector *v = HashSetElemVector(h, elemAddr);
  int pos = VectorSearch(v, elemAddr, h->comparefn, 0, false);
  return pos == -1 ? NULL : VectorNth(v, pos);
}
