#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "scanner.h"

#include "gen/ident.h"

void scanner_init(scanner_t *scanner, const char *source) {
  scanner->start = source;
  scanner->current = source;
  scanner->line_start = source;
  scanner->line = 1;
}

static token_t make_token(scanner_t *scanner, token_type_t type) {
  token_t token;
  token.type = type;
  token.start = scanner->start;
  token.length = (int)(scanner->current - scanner->start);
  token.line = scanner->line;
  token.column = (int)(scanner->start - scanner->line_start) + 1;
  return token;
}

static bool is_at_end(scanner_t *scanner) {
  return *scanner->current == '\0';
}

static token_t error_token(scanner_t *scanner, const char *message) {
  token_t token = make_token(scanner, TOKEN_ERROR);
  token.start = message;
  token.length = (int)strlen(message);
  return token;
}

static char advance(scanner_t *scanner) {
  scanner->current++;
  return scanner->current[-1];
}

static void advance_line(scanner_t *scanner) {
  scanner->line++;
  scanner->line_start = scanner->current;
}

static bool match(scanner_t *scanner, char expected) {
  if (is_at_end(scanner)) return false;
  if (*scanner->current != expected) return false;

  scanner->current++;
  return true;
}

static char peek(scanner_t *scanner) {
  return *scanner->current;
}

static char peek_next(scanner_t *scanner) {
  if (is_at_end(scanner)) return '\0';
  return scanner->current[1];
}

static token_t string(scanner_t *scanner) {
  while (peek(scanner) != '"' && !is_at_end(scanner)) {
    if (peek(scanner) == '\n') advance_line(scanner);
    advance(scanner);
  }

  if (is_at_end(scanner)) return error_token(scanner, "Unterminated string.");

  // Closing quote.
  advance(scanner);
  return make_token(scanner, TOKEN_STRING);
}

static bool is_digit(const char c) {
  return (c >= '0' && c <= '9');
}

static bool is_alpha(const char c) {
  return (c >= 'A' && c <= 'Z') ||
         (c >= 'a' && c <= 'z') ||
         (c == '_');
}

static token_t number(scanner_t *scanner) {
  while (is_digit(peek(scanner))) advance(scanner);

  // Look for fraction.
  if (peek(scanner) == '.' && is_digit(peek_next(scanner))) {
    // Consume "."
    advance(scanner);

    while (is_digit(peek(scanner))) advance(scanner);
  }

  return make_token(scanner, TOKEN_NUMBER);
}


static token_t identifier(scanner_t *scanner) {
  while (is_alpha(peek(scanner)) || is_digit(peek(scanner))) advance(scanner);

  return make_token(scanner, identifier_type(scanner));
}

static void skip_whitespace(scanner_t *scanner) {
  for (;;) {
    char c = peek(scanner);
    switch(c) {
      case ' ':
      case '\r':
      case '\t':
        advance(scanner);
        break;
      case '\n':
        advance_line(scanner);
        advance(scanner);
        break;
      case '/':
        if (peek_next(scanner) == '/') {
          // Comment goes to end of line.
          do {
            advance(scanner);
          } while (peek(scanner) != '\n' && !is_at_end(scanner));
        }
        else {
          return;
        }
        break;
      default:
        return;
    }
  }
}

token_t scan_token(scanner_t *scanner) {
#define make_token(type) make_token(scanner, type)
#define match(c) match(scanner, c)

  skip_whitespace(scanner);
  scanner->start = scanner->current;

  if (is_at_end(scanner)) {
    return make_token(TOKEN_EOF);
  }

  char c = advance(scanner);
  if (is_digit(c)) return number(scanner);
  if (is_alpha(c)) return identifier(scanner);

  switch(c) {
    case '(': return make_token(TOKEN_LEFT_PAREN);
    case ')': return make_token(TOKEN_RIGHT_PAREN);
    case '{': return make_token(TOKEN_LEFT_BRACE);
    case '}': return make_token(TOKEN_RIGHT_BRACE);
    case ';': return make_token(TOKEN_SEMICOLON);
    case ',': return make_token(TOKEN_COMMA);
    case '.': return make_token(TOKEN_DOT);
    case '-': return make_token(TOKEN_MINUS);
    case '+': return make_token(TOKEN_PLUS);
    case '/': return make_token(TOKEN_SLASH);
    case '*': return make_token(TOKEN_STAR);
    case '!':
      return make_token(
          match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
      return make_token(
          match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
      return make_token(
          match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
      return make_token(
          match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    case '"':
      return string(scanner);
  }

  return error_token(scanner, "Unexpected character.");
#undef make_token
#undef match
}
