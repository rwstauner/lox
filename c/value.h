#ifndef CLOX_VALUE_H
#define CLOX_VALUE_H

#include "common.h"
#include "memory.h"

typedef double value_t;

typedef struct value_array_t {
  array_meta_t meta;
  value_t* values;
} value_array_t;

void value_array_init(value_array_t* array);
void value_array_free(value_array_t* array);
void value_array_write(value_array_t* array, value_t value);

void value_print(value_t value);

#endif
