#include <stdio.h>

#include "chunk.h"
#include "debug.h"

int main(int argc, const char *argv[]) {
  chunk_t chunk;

  chunk_init(&chunk);

  chunk_write_constant(&chunk, 1.2, 123, 5);

  for (int i=0; i<(255 * 255); i++){
    chunk_write_constant(&chunk, (double)i, 125, 0);
  }

  chunk_write_instruction(&chunk, OP_RETURN, 123, 1);

  disassemble_chunk(&chunk, "test chunk");

  chunk_free(&chunk);

  return 0;
}
