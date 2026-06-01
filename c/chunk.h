#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include "common.h"
#include "memory.h"
#include "value.h"

typedef enum {
  OP_CONSTANT, // 1 opnd: constant index
  OP_RETURN,
} op_code_t;

typedef uint8_t byte_t;

// Useful for APIs but not stored this way.
// typedef struct source_location_t {
//   int line;
//   int column;
// } source_location_t;

typedef struct chunk_t {
  array_meta_t array_meta;
  byte_t *code;
  struct locations {
    array_meta_t line_meta;
    int *lines;
    int *line_counts; // Track how many times line elements are repeated.
  } locations;
  value_array_t constants;
} chunk_t;


void chunk_init(chunk_t *chunk);
void chunk_free(chunk_t *chunk);
void chunk_write(chunk_t *chunk, byte_t byte, int line);

#define chunk_write_instruction(chunk, byte, line) \
  chunk_write(chunk, byte, line)

#define chunk_write_data(chunk, byte) \
  chunk_write(chunk, byte, 0)

int chunk_add_constant(chunk_t* chunk, value_t value);
int chunk_get_line(chunk_t* chunk, int offset);
int chunk_instruction_length(byte_t instruction);

#endif
