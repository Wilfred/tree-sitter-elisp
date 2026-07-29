#include "tree_sitter/parser.h"

#include <stdbool.h>
#include <stdint.h>

enum TokenType {
  SYMBOL_WITH_RAW_ESCAPE,
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

static bool is_symbol_delimiter(int32_t character, bool initial) {
  return character == 0 || character == ' ' ||
         (character >= '\t' && character <= '\r') || character == '(' ||
         character == ')' || character == '[' || character == ']' ||
         character == '\'' || character == '`' || character == ',' ||
         character == '"' || character == ';' || character == '#' ||
         (initial && character == '?');
}

static bool scan_symbol_with_raw_escape(TSLexer *lexer, bool initial) {
  bool has_raw_escape = false;

  while (!lexer->eof(lexer)) {
    if (lexer->lookahead == '\\') {
      lexer->advance(lexer, false);
      if (lexer->eof(lexer)) {
        return false;
      }
      has_raw_escape =
          has_raw_escape || lexer->lookahead == 0 || lexer->lookahead == '\n';
      lexer->advance(lexer, false);
      initial = false;
    } else if (!is_symbol_delimiter(lexer->lookahead, initial)) {
      lexer->advance(lexer, false);
      initial = false;
    } else {
      break;
    }
  }

  if (!has_raw_escape) {
    return false;
  }

  lexer->result_symbol = SYMBOL_WITH_RAW_ESCAPE;
  return true;
}

bool tree_sitter_elisp_external_scanner_scan(void *payload, TSLexer *lexer,
                                             const bool *valid_symbols) {
  (void)payload;

  if (!valid_symbols[SYMBOL_WITH_RAW_ESCAPE]) {
    return false;
  }

  while (lexer->lookahead == ' ' ||
         (lexer->lookahead >= '\t' && lexer->lookahead <= '\r')) {
    lexer->advance(lexer, true);
  }

  if (lexer->lookahead == '#') {
    lexer->advance(lexer, false);
    if (lexer->lookahead != ':') {
      return false;
    }
    lexer->advance(lexer, false);
    return scan_symbol_with_raw_escape(lexer, false);
  }

  return scan_symbol_with_raw_escape(lexer, true);
}
