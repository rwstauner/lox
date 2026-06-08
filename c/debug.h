#ifndef CLOX_DEBUG_H
#define CLOX_DEBUG_H

#include "chunk.h"

static const int OFFSET_DIGITS = 4;
static const int COLUMN_DIGITS = 3;
static const int LINE_NUMBER_DIGITS = 4;

void disassemble_chunk(const chunk_t *chunk, const char *name);
int disassemble_instruction(const chunk_t *chunk, int insn_count, int offset);

#endif
