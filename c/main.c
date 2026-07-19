#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "debug.h"
#include "input.h"
#include "vm.h"

enum {
  EXIT_USAGE = 64,
  EXIT_COMPILE_ERROR = 65,
  EXIT_RUNTIME_ERROR = 70,
  EXIT_READ_FILE_ERROR = 74,
};

static void repl(vm_t *vm) {
  char *line;
  while ((line = read_line("lox> ")) != NULL) {
    if (*line) {
      // add_history(line);
      vm_interpret(vm, line);
    }
    free(line);
  }
  printf("\n");
}

char *read_file (const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Could not open file '%s'.\n", path);
    exit(EXIT_READ_FILE_ERROR);
  }

  fseek(file, 0L, SEEK_END);
  size_t size = ftell(file);
  rewind(file);

  char * buffer = (char *)malloc(size + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read file '%s'.\n", path);
  }

  size_t bytes_read = fread(buffer, sizeof(char), size, file);

  if (bytes_read < size){
    // TODO: count newlines
    fprintf(stderr, "Failed to read file %s past %ld.\n", path, bytes_read);
    exit(EXIT_READ_FILE_ERROR);
  }

  buffer[bytes_read] = '\0';

  fclose(file);
  return buffer;
}

static void run_file(vm_t *vm, const char *path) {
  char *source = read_file(path);
  interpret_result_t result = vm_interpret(vm, source);
  free(source);

  switch(result) {
    case INTERPRET_COMPILE_ERROR:
      exit(EXIT_COMPILE_ERROR);
    case INTERPRET_RUNTIME_ERROR:
      exit(EXIT_RUNTIME_ERROR);
    case INTERPRET_OK:
      break;
    default:
      fprintf(stderr, "Unknown interpreter result: %d\n", result);
  }
}

int main(int argc, const char *argv[]) {
  vm_t vm;
  vm_init(&vm);

  switch(argc - 1){
    case 0: repl(&vm); break;
    case 1: run_file(&vm, argv[1]); break;
    default:
      fprintf(stderr, "Usage: clox [path]\n");
      exit(EXIT_USAGE);
  }

  vm_free(&vm);
  return 0;
}
