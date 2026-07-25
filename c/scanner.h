#ifndef CLOX_SCANNER_H
#define CLOX_SCANNER_H

#include "gen/tokens.h"

typedef struct scanner_t {
  const char *start;
  const char *current;
  const char *line_start;
  int line;
} scanner_t;

typedef struct token_t {
  token_type_t type;
  const char *start;
  int length;
  int line;
  int column;
} token_t;

void scanner_init(scanner_t *scanner, const char* source);
token_t scan_token(scanner_t *scanner);

#endif
