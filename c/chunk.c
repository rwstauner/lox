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

  value_array_init(&chunk->constants);
}

void chunk_free(chunk_t *chunk) {
  ARRAY_FREE(&chunk->array_meta, chunk->code);
  ARRAY_FREE(&chunk->locations.line_meta, chunk->locations.lines);
  ARRAY_FREE(&chunk->locations.line_meta, chunk->locations.line_counts);
  value_array_free(&chunk->constants);
  chunk_init(chunk);
}

void chunk_write(chunk_t *chunk, byte_t byte, int line) {
  array_meta_t *am = &chunk->array_meta;
  if (am->capacity < am->count + 1) {
    int old_capacity = ARRAY_GROW_CAPACITY(am);
    ARRAY_GROW(am, chunk->code, old_capacity);
  }

  if (line > 0) {
    array_meta_t *lm = &chunk->locations.line_meta;
    if (lm->count == 0 ||
        chunk->locations.lines[lm->count - 1] != line) {
      if (lm->capacity < lm->count + 1) {
        int old_capacity = ARRAY_GROW_CAPACITY(lm);
        ARRAY_GROW(lm, chunk->locations.lines, old_capacity);
        ARRAY_GROW(lm, chunk->locations.line_counts, old_capacity);
      }

      chunk->locations.lines[lm->count] = line;
      chunk->locations.line_counts[lm->count] = 1;
      lm->count++;
    } else {
      chunk->locations.line_counts[lm->count - 1]++;
    }
  }

  chunk->code[am->count] = byte;
  am->count++;
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

int chunk_instruction_count(chunk_t* chunk, int offset) {
  int insn_count = 0;
  for (int i = 0; i <= offset;) {
    byte_t instruction = chunk->code[i];
    i += chunk_instruction_length(instruction);
    insn_count += 1;
  }
  return insn_count;
}

int chunk_get_line(chunk_t* chunk, int offset) {
  // NOTE: For the disassemble case we could track insn_count and pass it in instead of calculating it again.
  int insn_count = chunk_instruction_count(chunk, offset);

  int line_counts = 0;
  int line_index = 0;
  for (; line_index < chunk->locations.line_meta.count; line_index++) {
    if (line_counts + chunk->locations.line_counts[line_index] < insn_count){
      line_counts += chunk->locations.line_counts[line_index];
    } else {
      break;
    }
  }

  return chunk->locations.lines[line_index];
}
