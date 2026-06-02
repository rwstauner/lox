#include <stdio.h>

#include "chunk.h"
#include "debug.h"

int main(int argc, const char *argv[]) {
  chunk_t chunk;

  chunk_init(&chunk);

  int index = chunk_add_constant(&chunk, 1.2);
  chunk_write_instruction(&chunk, OP_CONSTANT, 123, 5);
  chunk_write_data(&chunk, index);

  chunk_write_instruction(&chunk, OP_RETURN, 123, 1);


  index = chunk_add_constant(&chunk, 2.4);
  chunk_write_instruction(&chunk, OP_CONSTANT, 125, 0);
  chunk_write_data(&chunk, index);


  disassemble_chunk(&chunk, "test chunk");

  chunk_free(&chunk);

  return 0;
}
