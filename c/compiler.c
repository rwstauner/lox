#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

typedef struct {
  scanner_t *scanner;
  token_t current;
  token_t previous;
  chunk_t *chunk;
  bool had_error;
  bool panic_mode;
} parser_t;

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT, // =
  PREC_OR,         // or
  PREC_AND,        // and
  PREC_EQUALITY,   // == !=
  PREC_COMPARISON, // < > <= >=
  PREC_TERM,       // + -
  PREC_FACTOR,     // * /
  PREC_UNARY,      // ! -
  PREC_CALL,       // . ()
  PREC_PRIMARY,
} precedence_t;

typedef void (*parse_fn_t)(parser_t *);

typedef struct {
  parse_fn_t prefix;
  parse_fn_t infix;
  precedence_t precedence;
} parse_rule_t;

static void parse_grouping(parser_t *);
static void parse_unary(parser_t *);
static void parse_binary(parser_t *);
static void parse_number(parser_t *);
static void parse_precedence(parser_t *, precedence_t);

parse_rule_t rules[] = {
  [TOKEN_LEFT_PAREN]    = {parse_grouping, NULL, PREC_NONE},
  [TOKEN_RIGHT_PAREN]   = {NULL, NULL, PREC_NONE},
  [TOKEN_LEFT_BRACE]    = {NULL, NULL, PREC_NONE},
  [TOKEN_RIGHT_BRACE]   = {NULL, NULL, PREC_NONE},
  [TOKEN_COMMA]         = {NULL, NULL, PREC_NONE},
  [TOKEN_DOT]           = {NULL, NULL, PREC_NONE},
  [TOKEN_MINUS]         = {parse_unary, parse_binary, PREC_TERM},
  [TOKEN_PLUS]          = {NULL, parse_binary, PREC_TERM},
  [TOKEN_SEMICOLON]     = {NULL, NULL, PREC_NONE},
  [TOKEN_SLASH]         = {NULL, parse_binary, PREC_FACTOR},
  [TOKEN_STAR]          = {NULL, parse_binary, PREC_FACTOR},
  [TOKEN_BANG]          = {NULL, NULL, PREC_NONE},
  [TOKEN_BANG_EQUAL]    = {NULL, NULL, PREC_NONE},
  [TOKEN_EQUAL]         = {NULL, NULL, PREC_NONE},
  [TOKEN_EQUAL_EQUAL]   = {NULL, NULL, PREC_NONE},
  [TOKEN_GREATER]       = {NULL, NULL, PREC_NONE},
  [TOKEN_GREATER_EQUAL] = {NULL, NULL, PREC_NONE},
  [TOKEN_LESS]          = {NULL, NULL, PREC_NONE},
  [TOKEN_LESS_EQUAL]    = {NULL, NULL, PREC_NONE},
  [TOKEN_IDENTIFIER]    = {NULL, NULL, PREC_NONE},
  [TOKEN_STRING]        = {NULL, NULL, PREC_NONE},
  [TOKEN_NUMBER]        = {parse_number, NULL, PREC_NONE},
  [TOKEN_AND]           = {NULL, NULL, PREC_NONE},
  [TOKEN_CLASS]         = {NULL, NULL, PREC_NONE},
  [TOKEN_ELSE]          = {NULL, NULL, PREC_NONE},
  [TOKEN_FALSE]         = {NULL, NULL, PREC_NONE},
  [TOKEN_FOR]           = {NULL, NULL, PREC_NONE},
  [TOKEN_FUN]           = {NULL, NULL, PREC_NONE},
  [TOKEN_IF]            = {NULL, NULL, PREC_NONE},
  [TOKEN_NIL]           = {NULL, NULL, PREC_NONE},
  [TOKEN_OR]            = {NULL, NULL, PREC_NONE},
  [TOKEN_PRINT]         = {NULL, NULL, PREC_NONE},
  [TOKEN_RETURN]        = {NULL, NULL, PREC_NONE},
  [TOKEN_SUPER]         = {NULL, NULL, PREC_NONE},
  [TOKEN_THIS]          = {NULL, NULL, PREC_NONE},
  [TOKEN_TRUE]          = {NULL, NULL, PREC_NONE},
  [TOKEN_VAR]           = {NULL, NULL, PREC_NONE},
  [TOKEN_WHILE]         = {NULL, NULL, PREC_NONE},
  [TOKEN_ERROR]         = {NULL, NULL, PREC_NONE},
  [TOKEN_EOF]           = {NULL, NULL, PREC_NONE},
};

static void error_at(parser_t *parser, token_t *token, const char *message) {
  if (parser->panic_mode) return;
  parser->panic_mode = true;

  fprintf(stderr, "[line %d] Error", token->line);

  if (token->type == TOKEN_EOF) {
    fprintf(stderr, " at end");
  }
  else if (token->type == TOKEN_ERROR) {
    // Nothing
  }
  else {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

  fprintf(stderr, ": %s\n", message);
  parser->had_error = true;
}

static void error_at_current(parser_t *parser, const char *message) {
  error_at(parser, &parser->current, message);
}

static void error(parser_t *parser, const char *message) {
  error_at(parser, &parser->previous, message);
}

static void advance(parser_t *parser) {
  parser->previous = parser->current;

  for (;;) {
    parser->current = scan_token(parser->scanner);
    if (parser->current.type != TOKEN_ERROR) break;

    error_at_current(parser, parser->current.start);
  }
}

static void consume(parser_t *parser, token_type_t type, const char *message) {
  if (parser->current.type == type) {
    advance(parser);
    return;
  }

  error_at_current(parser, message);
}

static void emit_byte(parser_t *parser, byte_t byte, token_t previous) {
  chunk_write_instruction(parser->chunk, byte, previous.line, previous.column);
}

// static void emit_2_bytes(parser_t *parser, byte_t b1, byte_t b2) {
//   emit_byte(parser, b1);
//   emit_byte(parser, b2);
// }

static void emit_constant(parser_t *parser, value_t value) {
  // TODO: error if past CONSTANT_BYTE_MAX?
  chunk_write_constant(parser->chunk, value, parser->previous.line, parser->previous.column);
}

static void emit_return(parser_t *parser) {
  emit_byte(parser, OP_RETURN, parser->previous);
}

static void end_compiler(parser_t *parser) {
  emit_return(parser);

#ifdef DEBUG_PRINT_CODE
  if (!parser->had_error) {
    disassemble_chunk(parser->chunk, "code");
  }
#endif
}

static void parse_expression(parser_t *parser) {
  parse_precedence(parser, PREC_ASSIGNMENT);
}

static void parse_grouping(parser_t *parser) {
  parse_expression(parser);
  consume(parser, TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void parse_unary(parser_t *parser) {
  token_t previous = parser->previous;
  token_type_t operator_type = previous.type;

  // Compile the operand.
  parse_precedence(parser, PREC_UNARY);

  // Emit operator instruction.
  switch(operator_type) {
    case TOKEN_MINUS: emit_byte(parser, OP_NEGATE, previous); break;
    default: return; // Unreachable.
  }
}

static parse_rule_t *get_rule(token_type_t type) {
  return &rules[type];
}

static void parse_binary(parser_t *parser) {
  token_t previous = parser->previous;
  token_type_t operator_type = previous.type;
  parse_rule_t *rule = get_rule(operator_type);
  parse_precedence(parser, (precedence_t)(rule->precedence + 1));

  switch(operator_type) {
    case TOKEN_PLUS:  emit_byte(parser, OP_ADD, previous); break;
    case TOKEN_MINUS: emit_byte(parser, OP_SUBTRACT, previous); break;
    case TOKEN_STAR:  emit_byte(parser, OP_MULTIPLY, previous); break;
    case TOKEN_SLASH: emit_byte(parser, OP_DIVIDE, previous); break;
    default: return; // Unreachable.
  }
}

static void parse_precedence(parser_t *parser, precedence_t precedence) {
  advance(parser);
  parse_fn_t prefix_rule = get_rule(parser->previous.type)->prefix;
  if (prefix_rule == NULL) {
    error(parser, "Expect expression.");
    return;
  }

  prefix_rule(parser);

  while (precedence <= get_rule(parser->current.type)->precedence) {
    advance(parser);
    parse_fn_t infix_rule = get_rule(parser->previous.type)->infix;
    infix_rule(parser);
  }
}

static void parse_number(parser_t *parser) {
  double value = strtod(parser->previous.start, NULL);
  emit_constant(parser, value);
}

static void parser_init(parser_t *parser, const char *source, chunk_t *chunk) {
  scanner_t *scanner = (scanner_t *)malloc(sizeof(scanner_t));
  scanner_init(scanner, source);

  parser->scanner = scanner;
  parser->had_error = false;
  parser->panic_mode = false;
  parser->chunk = chunk;
}

static void parser_free(parser_t *parser) {
  free(parser->scanner);
}

bool compile(const char* source, chunk_t *chunk) {
  parser_t parser;
  parser_init(&parser, source, chunk);

  advance(&parser);
  parse_expression(&parser);
  consume(&parser, TOKEN_EOF, "Expect end of expression.");

  end_compiler(&parser);
  parser_free(&parser);

  return !parser.had_error;
}

