#ifndef CLOX_OBJECT_OPS_H
#define CLOX_OBJECT_OPS_H

#include "object.h"

object_string_t *obj_copy_string(heap_t *heap, const char *chars, int length);
object_string_t *obj_take_string(heap_t *heap, char *chars, int length);

#endif
