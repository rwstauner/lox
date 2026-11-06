#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"

#define ALLOCATE_OBJ(heap, type, object_type) \
  (type *)allocate_object(heap, sizeof(type), object_type)

static object_t *allocate_object(heap_t *heap, size_t size, object_type_t type) {
  object_t *object = (object_t *)reallocate(NULL, 0, size);
  object->type = type;
  object->next = heap->objects;
  heap->objects = object;
  return object;
}

object_string_t *allocate_string(heap_t *heap, char *chars, int length) {
  object_string_t *string = ALLOCATE_OBJ(heap, object_string_t, OBJ_STRING);
  string->length = length;
  string->chars = chars;
  return string;
}

object_string_t *obj_copy_string(heap_t *heap, const char *chars, int length) {
  char *heap_chars = ALLOCATE(char, length + 1);
  memcpy(heap_chars, chars, length);
  heap_chars[length] = '\0';
  return allocate_string(heap, heap_chars, length);
}

object_string_t *obj_take_string(heap_t *heap, char *chars, int length) {
  return allocate_string(heap, chars, length);
}

void print_object(value_t value) {
  switch(OBJECT_TYPE(value)) {
    case OBJ_STRING:
      printf("\"%s\"", AS_CSTRING(value));
      break;
  }
}
