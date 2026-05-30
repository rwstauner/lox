#include <stdio.h>

#include "memory.h"
#include "value.h"

void value_array_init(value_array_t *array) {
  ARRAY_INIT(&array->meta);
  array->values = NULL;
}

void value_array_free(value_array_t *array) {
  ARRAY_FREE(&array->meta, array->values);
  value_array_init(array);
}

void value_array_write(value_array_t *array, value_t value) {
  array_meta_t *am = &array->meta;
  if (am->capacity < am->count + 1) {
    int old_capacity = ARRAY_GROW_CAPACITY(am);
    ARRAY_GROW(am, array->values, old_capacity);
  }

  array->values[am->count] = value;
  am->count++;
}

void value_print(value_t value) {
  printf("%g", value);
}
