#include <stdio.h>

#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char *argv[]) {
  vm_t vm;
  vm_init(&vm);

  chunk_t chunk;

  chunk_init(&chunk);

  chunk_write_constant(&chunk, 1.2, 123, 5);

  chunk_write_constant(&chunk, 3.4, 123, 5);

  chunk_write_instruction(&chunk, OP_ADD, 123, 6);

  chunk_write_constant(&chunk, 5.6, 123, 6);
  chunk_write_instruction(&chunk, OP_DIVIDE, 123, 4);
  chunk_write_instruction(&chunk, OP_NEGATE, 123, 4);

  // for (int i=0; i<(255 * 255); i++){
  //   chunk_write_constant(&chunk, (double)i, 125, 0);
  // }

  chunk_write_instruction(&chunk, OP_RETURN, 123, 1);

  disassemble_chunk(&chunk, "test chunk");

  printf("interpreting\n");
  interpret_result_t result = vm_interpret(&vm, &chunk);

  chunk_free(&chunk);

  return result;
}
