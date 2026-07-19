#include <stdio.h>

#include "common.h"
#include "debug.h"
#include "vm.h"

static void ensure_stack_capacity(vm_t *vm, int count) {
  array_meta_t *sm = &vm->stack.meta;
  if (sm->capacity < sm->count + count) {
    int old_capacity = ARRAY_GROW_CAPACITY(sm, count);
    int stack_pos = vm->stack.values == NULL ? 0 : vm->stack.top - vm->stack.values;
    ARRAY_GROW(sm, vm->stack.values, old_capacity);
    // Repoint stack to same position in new array.
    vm->stack.top = vm->stack.values + stack_pos;
  }
}

static void reset_stack(vm_t *vm) {
  ARRAY_INIT(&vm->stack.meta);
  vm->stack.values = NULL;
  ensure_stack_capacity(vm, 256);
}

static void push(vm_t *vm, value_t value) {
  ensure_stack_capacity(vm, 1);
  *vm->stack.top = value;
  vm->stack.top++;
}

static value_t pop(vm_t *vm) {
  vm->stack.top--;
  return *vm->stack.top;
}

void vm_init(vm_t *vm) {
  reset_stack(vm);
}

void vm_free(vm_t *vm) {
  (void)vm; // FIXME
}

static interpret_result_t run(vm_t *vm) {
#define READ_BYTE() (*vm->ip++)
#define READ_CONSTANT() (vm->chunk->constants.values[READ_BYTE()])
#define OFFSET() (int)(vm->ip - vm->chunk->code - 1)
#define BINARY_OP(op) do { \
    double b = pop(vm); \
    double a = pop(vm); \
    push(vm, a op b); \
  } while (0)
#define STACK_LAST() vm->stack.values[(vm->stack.top - vm->stack.values) - 1]

  for (int insn_count = 1; ; insn_count++) {

#ifdef DEBUG_TRACE_EXECUTION
    printf("%14s", " ");
    for (value_t *slot = vm->stack.values; slot < vm->stack.top; slot++) {
      printf("[ ");
      value_print(*slot);
      printf(" ]");
    }
    printf("\n");
    disassemble_instruction(vm->chunk,
                            insn_count,
                           (int)(vm->ip - vm->chunk->code));
#endif

    byte_t instruction;
    switch (instruction = READ_BYTE()) {
      case OP_CONSTANT_LONG: {
        int constant, size;
        value_t value = chunk_read_constant_long(vm->chunk, OFFSET(), &constant, &size);
        vm->ip += size - 1; // -1 because it's already been advanced

        push(vm, value);
        if(value > 16635){
          value_print(value);
          printf("\n");
        }
        break;
      }
      case OP_CONSTANT: {
        value_t value = READ_CONSTANT();
        push(vm, value);
        break;
      }
      case OP_ADD:      BINARY_OP(+); break;
      case OP_SUBTRACT: BINARY_OP(-); break;
      case OP_MULTIPLY: BINARY_OP(*); break;
      case OP_DIVIDE:   BINARY_OP(/); break;
      case OP_NEGATE: {
        STACK_LAST() = -STACK_LAST();
        break;
      }
      case OP_RETURN: {
        value_print(pop(vm));
        printf("\n");
        return INTERPRET_OK;
      }
      default: {
        printf("compile error\n");
        return INTERPRET_COMPILE_ERROR;
      }
    }
  }

#undef READ_CONSTANT
#undef READ_BYTE
}

interpret_result_t vm_interpret(vm_t *vm, const chunk_t *chunk) {
  vm->chunk = chunk;
  vm->ip = chunk->code;
  return run(vm);
}
