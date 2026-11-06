#ifndef CLOX_VALUE_H
#define CLOX_VALUE_H

#include "common.h"
#include "array_meta.h"

typedef enum {
  VAL_NIL,
  VAL_BOOL,
  VAL_NUMBER,
  VAL_OBJECT,
} value_type_t;

typedef struct object_t object_t;
typedef struct object_string_t object_string_t;

typedef struct value_t {
  value_type_t type;
  union {
    bool boolean;
    double number;
    object_t *obj;
  } as;
} value_t;

#define AS_BOOL(value)   ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)
#define AS_OBJECT(value)    ((value).as.obj)

#define NIL_VALUE           ((value_t){VAL_NIL,    {.number  = 0}})
#define BOOL_VALUE(value)   ((value_t){VAL_BOOL,   {.boolean = (value)}})
#define NUMBER_VALUE(value) ((value_t){VAL_NUMBER, {.number  = (value)}})
#define OBJECT_VALUE(value)    ((value_t){VAL_OBJECT, {.obj     = (object_t *)(value)}})

#define IS_NIL(value)    ((value).type == VAL_NIL)
#define IS_BOOL(value)   ((value).type == VAL_BOOL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)
#define IS_OBJECT(value)    ((value).type == VAL_OBJECT)

typedef struct value_array_t {
  array_meta_t meta;
  value_t *values;
} value_array_t;

void value_array_init(value_array_t* array);
void value_array_free(value_array_t* array);
void value_array_write(value_array_t* array, value_t value);

bool values_equal(value_t a, value_t b);
void value_print(value_t value);

#endif
