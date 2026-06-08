#ifndef CLOX_MEMORY_H
#define CLOX_MEMORY_H

#include "common.h"

typedef struct array_meta_t {
  int count;
  int capacity;
} array_meta_t;

#define ARRAY_INIT(meta) \
  ((meta)->capacity = 0, (meta)->count = 0)

static inline int array_grow_capacity(array_meta_t *array, int count) {
  int old = array->capacity;
  while(array->capacity < old + count) {
    array->capacity = array->capacity < 8 ? 8 : array->capacity * 2;
  }
  return old;
}

// Separate capacity growth from array growth
// to allow multiple arrays to share one meta object.

#define ARRAY_GROW_CAPACITY(meta, count) \
  array_grow_capacity(meta, count)

void *reallocate(void *pointer, size_t old_size, size_t new_size);

#define ARRAY_GROW(meta, ptr, old_count) \
  (ptr) = reallocate((ptr), \
    sizeof(*(ptr)) * (old_count), \
    sizeof(*(ptr)) * (meta)->capacity)

#define ARRAY_FREE(meta, ptr) \
    reallocate((ptr), sizeof(*(ptr)) * (meta)->capacity, 0)

#endif
