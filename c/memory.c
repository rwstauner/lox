#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "memory.h"

void *reallocate(void *pointer, size_t old_size, size_t new_size) {
  if (new_size == 0) {
    DEBUG_PRINT("FREE(%p) ", pointer);
    free(pointer);
    return NULL;
  }
  (void)old_size; // FIXME

  void *result = realloc(pointer, new_size);
  if (result == NULL){
    fprintf(stderr, "Failed to allocate\n");
    exit(1);
  }
  return result;
}

static void free_object(object_t *object) {
  switch (object->type) {
    case OBJ_STRING: {
      DEBUG_PRINT("Freeing string { ");
      print_object(OBJECT_VALUE(object));
      DEBUG_PRINT(" ");
      FREE_EXTRA(object_string_t, object, ((object_string_t*)object)->length + 1);
      DEBUG_PRINT(" }\n");
      break;
    }
  }
}

void free_objects(object_t *objects) {
  object_t *object = objects;
  while (object != NULL) {
    object_t *next = object->next;
    free_object(object);
    object = next;
  }
}
