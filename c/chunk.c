#include <stdlib.h>
#include <stdio.h>

#include "chunk.h"
#include "memory.h"

void chunk_init(chunk_t *chunk) {
  ARRAY_INIT(&chunk->array_meta);
  chunk->code = NULL;

  ARRAY_INIT(&chunk->locations.line_meta);
  chunk->locations.lines = NULL;
  chunk->locations.line_counts = NULL;

  ARRAY_INIT(&chunk->locations.column_meta);
  chunk->locations.columns = NULL;

  value_array_init(&chunk->constants);
}

void chunk_free(chunk_t *chunk) {
  ARRAY_FREE(&chunk->array_meta, chunk->code);
  ARRAY_FREE(&chunk->locations.line_meta, chunk->locations.lines);
  ARRAY_FREE(&chunk->locations.line_meta, chunk->locations.line_counts);
  ARRAY_FREE(&chunk->locations.column_meta, chunk->locations.columns);
  value_array_free(&chunk->constants);
  chunk_init(chunk);
}

void chunk_write_data(chunk_t *chunk, int count, byte_t bytes[]) {
  array_meta_t *am = &chunk->array_meta;
  if (am->capacity < am->count + count) {
    int old_capacity = ARRAY_GROW_CAPACITY(am, count);
    ARRAY_GROW(am, chunk->code, old_capacity);
  }

  for ( int i = 0; i < count; i++ ){
    chunk->code[am->count] = bytes[i];
    am->count++;
  }
}

void chunk_write_instruction(chunk_t *chunk, byte_t byte, int line, int column) {
  chunk_write_data(chunk, 1, &byte);

  {
    array_meta_t *lm = &chunk->locations.line_meta;
    if (lm->count == 0 ||
        chunk->locations.lines[lm->count - 1] != line) {
      if (lm->capacity < lm->count + 1) {
        int old_capacity = ARRAY_GROW_CAPACITY(lm, 1);
        ARRAY_GROW(lm, chunk->locations.lines, old_capacity);
        ARRAY_GROW(lm, chunk->locations.line_counts, old_capacity);
      }

      chunk->locations.lines[lm->count] = line;
      chunk->locations.line_counts[lm->count] = 1;
      lm->count++;
    }
    else {
      chunk->locations.line_counts[lm->count - 1]++;
    }
  }

  {
    array_meta_t *cm = &chunk->locations.column_meta;
    if (cm->capacity < cm->count + 1) {
      int old_capacity = ARRAY_GROW_CAPACITY(cm, 1);
      ARRAY_GROW(cm, chunk->locations.columns, old_capacity);
    }

    chunk->locations.columns[cm->count] = column;
    cm->count++;
  }
}

// Add value to chunk constants and return the index where it was added.
static int chunk_add_constant(chunk_t* chunk, value_t value) {
  value_array_write(&chunk->constants, value);
  return chunk->constants.meta.count - 1;
}

value_t chunk_read_constant_long(const chunk_t *chunk, int offset, int *out_constant, int *out_size) {
  int constant = 0;
  byte_t bits = 0;
  int index = offset + 1;
  do {
    bits = chunk->code[index];
    constant = (constant << 7) + (bits & 127);
    index++;
  } while (bits & (1 << 7));

  // OP_CONSTANT_LONG starts after 255.
  constant += 255;

  *out_constant = constant;
  *out_size = index - offset;

  return chunk->constants.values[constant];
}

#define CONSTANT_BYTE_MAX 8

void chunk_write_constant(chunk_t *chunk, value_t value, int line, int column) {
  int index = chunk_add_constant(chunk, value);
  int op = OP_CONSTANT;
  int byte_count = 0;
  byte_t bytes[CONSTANT_BYTE_MAX];

  if (index < 256) {
    byte_count = 1;
    bytes[CONSTANT_BYTE_MAX - 1] = index;
  }
  else {
    // Constants are all stored in the same array.
    // OP_CONSTANT uses one byte for the value.
    // OP_CONSTANT_LONG can use up to 7 bytes to indicate how far the index is past 255.
    op = OP_CONSTANT_LONG;
    index -= 255;
    int mask = 0; // Lowest 7bits will have no top marker.
    do {
      byte_count++;
      // Move lowest 7 bits into slots from right to left.
      bytes[CONSTANT_BYTE_MAX - byte_count] = index & 127 | mask;
      index >>= 7;
      mask = (1 << 7); // Remaining bits will get the continuation bit set.
    } while(index > 0);
  }
  chunk_write_instruction(chunk, op, line, column);
  chunk_write_data(chunk, byte_count, (bytes + (CONSTANT_BYTE_MAX - byte_count)));
}

source_location_t *chunk_get_location(const chunk_t* chunk, int insn_count, source_location_t *out_location) {
  int line_index = 0;
  for (int line_counts = 0; line_index < chunk->locations.line_meta.count; line_index++) {
    if (line_counts + chunk->locations.line_counts[line_index] < insn_count){
      line_counts += chunk->locations.line_counts[line_index];
    }
    else {
      break;
    }
  }

  out_location->line = chunk->locations.lines[line_index];
  // insn:col is 1:1
  out_location->column = chunk->locations.columns[insn_count - 1];

  return out_location;
}
