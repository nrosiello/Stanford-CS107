#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void VectorGrow(vector *v)
{
  v->allocLength += v->initialAllocation;
  v->elems = realloc(v->elems, v->allocLength * v->elemSize);
  assert(v->elems != NULL);
}

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
  assert(elemSize > 0);
  v->elemSize = elemSize;
  
  assert(initialAllocation >= 0);
  if (initialAllocation == 0)
    initialAllocation = 10;
 
  v->allocLength = initialAllocation;
  v->initialAllocation = initialAllocation;
  
  v->logLength = 0;
  v->freeFn = freeFn;
  v->elems = malloc(v->allocLength * elemSize);
  assert(v->elems != NULL);
}

void VectorDispose(vector *v)
{}

int VectorLength(const vector *v)
{ return 0; }

void *VectorNth(const vector *v, int position)
{
  assert(position >= 0);
  assert(position < v->logLength);
  return (char *) v->elems + (position * v->elemSize); 
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{}

void VectorInsert(vector *v, const void *elemAddr, int position)
{}

void VectorAppend(vector *v, const void *elemAddr)
{
  if (v->allocLength == v->logLength)
    VectorGrow(v);  

  v->logLength++;
  void *newElem = VectorNth(v, v->logLength - 1);
  memcpy(newElem, elemAddr, v->elemSize);
}

void VectorDelete(vector *v, int position)
{}

void VectorSort(vector *v, VectorCompareFunction compare)
{}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
  assert(mapFn != NULL);
  for (int i = 0; i < v->logLength; i++)
    mapFn(VectorNth(v, i), auxData);
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{ return -1; } 
