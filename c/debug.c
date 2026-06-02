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

  source_location_t location = *chunk_get_location(chunk, offset, &location);
  source_location_t previous;

  if (offset > 0 && location.line == chunk_get_location(chunk, offset - 1, &previous)->line) {
    printf("%*s", LINE_NUMBER_DIGITS, "|");
  }
  else {
    printf("%*d", LINE_NUMBER_DIGITS, location.line);
  }
  if (location.column > 0) {
    printf(":%0*d ", COLUMN_DIGITS, location.column);
  }
  else {
    printf(" %*s ", COLUMN_DIGITS, " ");
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
