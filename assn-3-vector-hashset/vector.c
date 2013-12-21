#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <search.h>

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
{
  if (v->freeFn != NULL) {
    for (int i = 0; i < v->logLength; i++)
      v->freeFn(VectorNth(v, i));
  }
 
  free(v->elems);
} 

int VectorLength(const vector *v)
{ 
  return v->logLength;
}

// the raw Nth function does not do any error checking.  it is meant for internal
// use by the vector implementation
static void *VectorRawNth(const vector *v, int position)
{
  return (char *) v->elems + (position * v->elemSize); 
}

void *VectorNth(const vector *v, int position)
{
  assert(position >= 0);
  assert(position < v->logLength);
  return VectorRawNth(v, position);
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
  assert(position > 0);
  assert(position < v->logLength);

  if (v->freeFn != NULL)
    v->freeFn(VectorNth(v, position));

  memcpy(VectorNth(v, position), elemAddr, v->elemSize);
}

static void VectorElemShift(vector *v, int position, int delta)
{
  void *src = VectorRawNth(v, position);
  void *dest = VectorRawNth(v, position + delta);
  size_t len = (char *) VectorRawNth(v, v->logLength) - (char *) src;
  memmove(dest, src, len);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
  assert(position >= 0);
  assert(position <= v->logLength);

  if (v->allocLength == v->logLength)
    VectorGrow(v);
  
  // shift the elements over as necessary
  VectorElemShift(v, position, 1);

  // insert the new element into its slot
  memcpy(VectorRawNth(v, position), elemAddr, v->elemSize);
  v->logLength++;
}

void VectorAppend(vector *v, const void *elemAddr)
{
  if (v->allocLength == v->logLength)
    VectorGrow(v);  

  v->logLength++;
  void *newElem = VectorNth(v, v->logLength - 1);
  memcpy(newElem, elemAddr, v->elemSize);
}

void VectorDelete(vector *v, int position)
{
  assert(position >= 0);
  assert(position < v->logLength);

  if (v->freeFn != NULL)
    v->freeFn(VectorNth(v, position));
  
  VectorElemShift(v, position+1, -1);
  v->logLength--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
  assert(compare != NULL);
  qsort(v->elems, v->logLength, v->elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
  assert(mapFn != NULL);
  for (int i = 0; i < v->logLength; i++)
    mapFn(VectorNth(v, i), auxData);
}

// Given a pointer to an element in the array, returns the integer index of 
// the element.
static int VectorIndex(const vector *v, const void *elemAddr)
{
  int offset = (char *) elemAddr - (char *) v->elems;
  return offset / v->elemSize;
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{ 
  assert(key != NULL);
  assert(searchFn != NULL);
  assert(startIndex >= 0);
  assert(startIndex < v->logLength);

  void *found;
  void *base = VectorNth(v, startIndex);
  size_t numElems = v->logLength - startIndex;
  if (isSorted) {
    found = bsearch(key, base, numElems, v->elemSize, searchFn);
  } else {
    found = lfind(key, base, &numElems, v->elemSize, searchFn);
  }

  return found ? VectorIndex(v, found) : kNotFound;
}
