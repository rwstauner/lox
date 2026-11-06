#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
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
    int old_capacity = ARRAY_GROW_CAPACITY(am, 1);
    ARRAY_GROW(am, array->values, old_capacity);
  }

  array->values[am->count] = value;
  am->count++;
}

bool values_equal(value_t a, value_t b) {
  if (a.type != b.type) return false;
  switch(a.type) {
    case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NIL:  return true;
    case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
    case VAL_OBJECT: {
      object_string_t *a_str = AS_STRING(a);
      object_string_t *b_str = AS_STRING(b);
      return (a_str->length == b_str->length &&
          0 == memcmp(a_str->chars,
                      b_str->chars,
                      a_str->length));
    }
    default: return false; // Unreachable.
  }
}

void value_print(value_t value) {
  switch(value.type) {
    case VAL_BOOL:
      printf("%s", AS_BOOL(value) ? "true" : "false");
      break;
    case VAL_NIL:
      printf("%s", "nil");
      break;
    case VAL_NUMBER:
      printf("%g", AS_NUMBER(value));
      break;
    case VAL_OBJECT:
      print_object(value);
      break;
    default:
      printf("%s", "value_print what?");
      break;
  }
}
