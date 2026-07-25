#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include "common.h"
#include "memory.h"
#include "value.h"

typedef enum {
  OP_CONSTANT, // 1 opnd: constant index
  OP_CONSTANT_LONG, // 1 opnd: constant index
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NEGATE,
  OP_RETURN,
} op_code_t;

typedef uint8_t byte_t;

#define CONSTANT_BYTE_MAX 8

// Useful for APIs but not stored this way.
typedef struct source_location_t {
  // const char *file;
  int line;
  int column;
} source_location_t;

typedef struct chunk_t {
  array_meta_t array_meta;
  byte_t *code;
  struct locations {
    array_meta_t line_meta;
    int *lines;
    int *line_counts; // Track how many times line elements are repeated.
    // Use separate arrays for column and line to reduce duplicate storage.
    array_meta_t column_meta;
    int *columns;
  } locations;
  value_array_t constants;
} chunk_t;


void chunk_init(chunk_t *chunk);
void chunk_free(chunk_t *chunk);

// TODO: Take source_location_t?

void chunk_write_constant(chunk_t *chunk, value_t value, int line, int column);
void chunk_write_data(chunk_t *chunk, int count, byte_t bytes[]);
void chunk_write_instruction(chunk_t *chunk, byte_t byte, int line, int column);

value_t chunk_read_constant_long(const chunk_t* chunk, int offset, int *out_constant, int *out_size);
source_location_t *chunk_get_location(const chunk_t* chunk, int offset, source_location_t *out_location);
int chunk_instruction_length(byte_t instruction);

#endif
