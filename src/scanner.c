#include "tree_sitter/parser.h"

#include <stdbool.h>

enum TokenType {
  COMMENT,
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

bool tree_sitter_elisp_external_scanner_scan(void *payload, TSLexer *lexer,
                                             const bool *valid_symbols) {
  (void)payload;

  if (!valid_symbols[COMMENT]) {
    return false;
  }

  while (lexer->lookahead == ' ' ||
         (lexer->lookahead >= '\t' && lexer->lookahead <= '\r')) {
    lexer->advance(lexer, true);
  }

  if (lexer->lookahead != ';') {
    return false;
  }

  do {
    lexer->advance(lexer, false);
  } while (!lexer->eof(lexer) && lexer->lookahead != '\n');

  lexer->result_symbol = COMMENT;
  return true;
}
