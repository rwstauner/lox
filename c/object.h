#ifndef CLOX_OBJECT_H
#define CLOX_OBJECT_H

#include "common.h"
#include "value.h"

typedef enum {
  OBJ_STRING,
} object_type_t;

struct object_t {
  object_type_t type;
  struct object_t *next;
};

struct object_string_t {
  object_t obj;
  int length;
  char chars[];
};

#define OBJECT_TYPE(value) (AS_OBJECT(value)->type)
#define IS_STRING(value)   (obj_is_type(value, OBJ_STRING) )
#define AS_STRING(value)   ((object_string_t *)AS_OBJECT(value))

static inline bool obj_is_type(value_t value, object_type_t type) {
  return IS_OBJECT(value) && AS_OBJECT(value)->type == type;
}

void print_object(value_t value);

#endif
