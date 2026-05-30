#include <stdlib.h>
#include <stdio.h>

#include "chunk.h"
#include "memory.h"

void chunk_init(chunk_t *chunk) {
  ARRAY_INIT(&chunk->array_meta);
  chunk->code = NULL;
  chunk->lines = NULL;
  value_array_init(&chunk->constants);
}

void chunk_free(chunk_t *chunk) {
  ARRAY_FREE(&chunk->array_meta, chunk->code);
  ARRAY_FREE(&chunk->array_meta, chunk->lines);
  value_array_free(&chunk->constants);
  chunk_init(chunk);
}

void chunk_write(chunk_t *chunk, byte_t byte, int line) {
  array_meta_t *am = &chunk->array_meta;
  if (am->capacity < am->count + 1) {
    int old_capacity = ARRAY_GROW_CAPACITY(am);
    ARRAY_GROW(am, chunk->code, old_capacity);
    ARRAY_GROW(am, chunk->lines, old_capacity);
  }

  chunk->code[am->count] = byte;
  chunk->lines[am->count] = line;
  am->count++;
}

// Add value to chunk constants and return the index where it was added.
int chunk_add_constant(chunk_t* chunk, value_t value) {
  value_array_write(&chunk->constants, value);
  return chunk->constants.meta.count - 1;
}
