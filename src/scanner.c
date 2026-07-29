#include "tree_sitter/parser.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum TokenType {
  BYTECODE_COMMENT,
};

void *tree_sitter_elisp_external_scanner_create(void) { return NULL; }

void tree_sitter_elisp_external_scanner_destroy(void *payload) {
  (void)payload;
}

unsigned tree_sitter_elisp_external_scanner_serialize(void *payload,
                                                      char *buffer) {
  (void)payload;
  (void)buffer;
  return 0;
}

void tree_sitter_elisp_external_scanner_deserialize(void *payload,
                                                    const char *buffer,
                                                    unsigned length) {
  (void)payload;
  (void)buffer;
  (void)length;
}

static bool scan_bytecode_comment(TSLexer *lexer) {
  size_t count = 0;
  bool has_digit = false;
  while (lexer->lookahead >= '0' && lexer->lookahead <= '9') {
    unsigned digit = lexer->lookahead - '0';
    if (count > (SIZE_MAX - digit) / 10) {
      return false;
    }
    count = count * 10 + digit;
    has_digit = true;
    lexer->advance(lexer, false);
  }
  if (!has_digit) {
    return false;
  }

  size_t consumed = 0;
  while (consumed < count) {
    if (lexer->eof(lexer)) {
      return false;
    }
    size_t width = 1;
    if (lexer->lookahead > 0x7f && lexer->lookahead <= 0x7ff) {
      width = 2;
    } else if (lexer->lookahead > 0x7ff && lexer->lookahead <= 0xffff) {
      width = 3;
    } else if (lexer->lookahead > 0xffff) {
      width = 4;
    }
    if (consumed + width > count) {
      return false;
    }
    lexer->advance(lexer, false);
    consumed += width;
  }

  lexer->result_symbol = BYTECODE_COMMENT;
  return true;
}

bool tree_sitter_elisp_external_scanner_scan(void *payload, TSLexer *lexer,
                                             const bool *valid_symbols) {
  (void)payload;

  if (!valid_symbols[BYTECODE_COMMENT]) {
    return false;
  }

  while (lexer->lookahead == ' ' ||
         (lexer->lookahead >= '\t' && lexer->lookahead <= '\r')) {
    lexer->advance(lexer, true);
  }

  if (lexer->lookahead != '#') {
    return false;
  }
  lexer->advance(lexer, false);
  if (lexer->lookahead != '@') {
    return false;
  }
  lexer->advance(lexer, false);
  return scan_bytecode_comment(lexer);
}
