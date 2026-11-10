#ifndef CLOX_OBJECT_OPS_H
#define CLOX_OBJECT_OPS_H

#include "object.h"

object_string_t *allocate_string(heap_t *heap, int length);
object_string_t *obj_copy_string(heap_t *heap, const char *chars, int length);
object_string_t *string_intern(heap_t *heap, object_string_t *string);

uint32_t hash_string(const char *chars, int length);

#endif
