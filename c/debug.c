#include <stdio.h>

#include "debug.h"
#include "value.h"

void disassemble_chunk(chunk_t *chunk, const char *name) {
  printf("== %s ==\n", name);

  for (int offset = 0; offset < chunk->array_meta.count;) {
    offset = disassemble_instruction(chunk, offset);
  }
}

static void simple_instruction(const char *name, int offset) {
  printf("%s\n", name);
}

static void constant_instruction(const char *name, chunk_t* chunk, int offset) {
  byte_t constant = chunk->code[offset + 1];
  printf("%-16s %*d '", name, OFFSET_DIGITS, constant);
  value_print(chunk->constants.values[constant]);
  printf("'\n");
}

int disassemble_instruction(chunk_t *chunk, int offset) {
  printf("%0*d ", OFFSET_DIGITS, offset);

  int line = chunk_get_line(chunk, offset);
  if (offset > 0 && line == chunk_get_line(chunk, offset - 1)) {
    printf("%*s ", LINE_NUMBER_DIGITS, "|");
  }
  else {
    printf("%*d ", LINE_NUMBER_DIGITS, line);
  }

  byte_t instruction = chunk->code[offset];
  switch (instruction) {
    case OP_CONSTANT:
      constant_instruction("OP_CONSTANT", chunk, offset);
      break;
    case OP_RETURN:
      simple_instruction("OP_RETURN", offset);
      break;
    default:
      printf("Unknown opcode %d\n", instruction);
      break;
  }

  return offset + chunk_instruction_length(instruction);
}
