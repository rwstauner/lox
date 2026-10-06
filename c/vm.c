#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "compiler.h"
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

static void runtime_error(vm_t *vm, int insn_count, const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  source_location_t location = *chunk_get_location(vm->chunk, insn_count, &location);
  fprintf(stderr, "[at %d:%d] in script\n", location.line, location.column);
  reset_stack(vm);
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
  free(vm->stack.values);
}

static bool is_falsey(value_t value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static interpret_result_t run(vm_t *vm) {
#define READ_BYTE() (*vm->ip++)
#define READ_CONSTANT() (vm->chunk->constants.values[READ_BYTE()])
#define OFFSET() (int)(vm->ip - vm->chunk->code - 1)
#define PEEK(offset) vm->stack.values[(vm->stack.top - vm->stack.values) - 1 - (offset)]
#define BINARY_OP(value_type, op) do { \
    if (!IS_NUMBER(PEEK(0)) || !IS_NUMBER(PEEK(1))) { \
      runtime_error(vm, insn_count, "Operands must be numbers."); \
      return INTERPRET_RUNTIME_ERROR; \
    } \
    double b = AS_NUMBER(pop(vm)); \
    double a = AS_NUMBER(pop(vm)); \
    push(vm, value_type(a op b)); \
  } while (0)
#define STACK_LAST() PEEK(0)

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
        if(AS_NUMBER(value) > 16635){
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

      case OP_NIL:      push(vm ,NIL_VALUE); break;
      case OP_FALSE:    push(vm ,BOOL_VALUE(false)); break;
      case OP_TRUE:     push(vm ,BOOL_VALUE(true)); break;

      case OP_EQUAL: {
        value_t a = pop(vm);
        value_t b = pop(vm);
        push(vm ,BOOL_VALUE(values_equal(a, b)));
        break;
      }
      case OP_GREATER:  BINARY_OP(BOOL_VALUE, >); break;
      case OP_LESS:     BINARY_OP(BOOL_VALUE, <); break;

      case OP_ADD:      BINARY_OP(NUMBER_VALUE, +); break;
      case OP_SUBTRACT: BINARY_OP(NUMBER_VALUE, -); break;
      case OP_MULTIPLY: BINARY_OP(NUMBER_VALUE, *); break;
      case OP_DIVIDE:   BINARY_OP(NUMBER_VALUE, /); break;

      case OP_NOT:
        push(vm ,BOOL_VALUE(is_falsey(pop(vm))));
        break;
      case OP_NEGATE: {
        if (!IS_NUMBER(STACK_LAST())) {
          runtime_error(vm, insn_count, "Operand must be a number.");
          return INTERPRET_RUNTIME_ERROR;
        }
        STACK_LAST() = NUMBER_VALUE(-AS_NUMBER(STACK_LAST()));
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

interpret_result_t vm_interpret(vm_t *vm, const char *source) {
  chunk_t chunk;
  chunk_init(&chunk);

  if (!compile(source, &chunk)) {
    chunk_free(&chunk);
    return INTERPRET_COMPILE_ERROR;
  }

  vm->chunk = &chunk;
  vm->ip = chunk.code;
  interpret_result_t result = run(vm);

  chunk_free(&chunk);
  return result;
}
