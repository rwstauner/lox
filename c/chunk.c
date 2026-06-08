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
int chunk_add_constant(chunk_t* chunk, value_t value) {
  value_array_write(&chunk->constants, value);
  return chunk->constants.meta.count - 1;
}

int chunk_instruction_length(byte_t instruction) {
  switch (instruction) {
    case OP_CONSTANT:
      return 2;
    case OP_RETURN:
      return 1;
    default:
      fprintf(stderr, "Unknown opcode %d\n", instruction);
      return 1;
  }
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
