#ifndef CLOX_VM_H
#define CLOX_VM_H

#include "chunk.h"
#include "memory.h"
#include "value.h"

typedef struct stack_t {
  array_meta_t meta;
  value_t *values;
  value_t *top;
} stack_t;

typedef struct vm_t {
  const chunk_t *chunk;
  byte_t *ip; // instruction pointer
  stack_t stack;
} vm_t;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR,
} interpret_result_t;

void vm_init(vm_t *vm);
void vm_free(vm_t *vm);
interpret_result_t vm_interpret(vm_t *vm, const chunk_t *chunk);

#endif
