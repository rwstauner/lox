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

typedef struct {
  array_meta_t array_meta;
  byte_t *code;
  int *lines;
  value_array_t constants;
} chunk_t;

void chunk_init(chunk_t *chunk);
void chunk_free(chunk_t *chunk);
void chunk_write(chunk_t *chunk, byte_t byte, int line);
int chunk_add_constant(chunk_t* chunk, value_t value);

#endif
