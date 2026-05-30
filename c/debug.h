#ifndef CLOX_DEBUG_H
#define CLOX_DEBUG_H

#include "chunk.h"

#define OFFSET_DIGITS 4
#define LINE_NUMBER_DIGITS 4

void disassemble_chunk(chunk_t *chunk, const char *name);
int disassemble_instruction(chunk_t *chunk, int offset);

#endif
