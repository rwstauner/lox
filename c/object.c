#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"

#define ALLOCATE_OBJ(heap, type, extra, object_type) \
  (type *)allocate_object(heap, sizeof(type) + extra, object_type)

uint32_t hash_string(const char *chars, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)chars[i];
    hash *= 16777619;
  }
  return hash;
}

static object_t *allocate_object(heap_t *heap, size_t size, object_type_t type) {
  object_t *object = (object_t *)reallocate(NULL, 0, size);
  object->type = type;
  object->next = heap->objects;
  heap->objects = object;
  return object;
}

object_string_t *allocate_string(heap_t *heap, int length) {
  object_string_t *string = ALLOCATE_OBJ(heap, object_string_t, sizeof(char) * (length + 1), OBJ_STRING);
  string->length = length;
  string->chars[length] = '\0';
  return string;
}

object_string_t *obj_copy_string(heap_t *heap, const char *chars, int length) {
  uint32_t hash = hash_string(chars, length);
  object_string_t *interned = table_find_string(&heap->strings, chars, length, hash);
  if (interned != NULL) return interned;

  object_string_t *string = allocate_string(heap, length);
  memcpy(string->chars, chars, length);
  string->hash = hash;
  return string;
}

void print_object(value_t value) {
  switch(OBJECT_TYPE(value)) {
    case OBJ_STRING: {
      object_string_t *string = AS_STRING(value);
      printf("\"%.*s\"", string->length, string->chars);
      break;
    }
  }
}

// TODO: just make this string an output param?
object_string_t *string_intern(heap_t *heap, object_string_t *string) {
  object_string_t *interned = table_find_string(&heap->strings, string->chars, string->length, string->hash);

  if (interned != NULL) {
    printf("exchanged for interned string\n");
    // TODO: free string?
    return interned;
  }

  table_set(&heap->strings, string, NIL_VALUE);
  return string;
}
