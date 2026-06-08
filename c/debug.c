#include <stdio.h>

#include "debug.h"
#include "value.h"

void disassemble_chunk(const chunk_t *chunk, const char *name) {
  printf("== %s ==\n", name);

  int insn_count = 0;
  for (int offset = 0; offset < chunk->array_meta.count;) {
    insn_count++;
    offset = disassemble_instruction(chunk, insn_count, offset);
  }
}

static int simple_instruction(const char *name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}

static int constant_instruction(const char *name, const chunk_t* chunk, int offset) {
  byte_t constant = chunk->code[offset + 1];
  printf("%-16s %*d '", name, OFFSET_DIGITS, constant);
  value_print(chunk->constants.values[constant]);
  printf("'\n");
  return offset + 2;
}

int disassemble_instruction(const chunk_t *chunk, int insn_count, int offset) {
  printf("%0*d ", OFFSET_DIGITS, offset);

  source_location_t location = *chunk_get_location(chunk, insn_count, &location);
  source_location_t previous;

  if (insn_count > 1 && location.line == chunk_get_location(chunk, insn_count - 1, &previous)->line) {
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
      return constant_instruction("OP_CONSTANT", chunk, offset);
    case OP_RETURN:
      return simple_instruction("OP_RETURN", offset);
    default:
      printf("Unknown opcode %d\n", instruction);
      return offset + 1;
  }
}
