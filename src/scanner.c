#include "tree_sitter/parser.h"

#include <stdbool.h>
#include <stdint.h>

enum TokenType {
  INTEGER_WITH_BASE,
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

static int digit_value(int32_t character) {
  if (character >= '0' && character <= '9') {
    return character - '0';
  }
  if (character >= 'A' && character <= 'Z') {
    return character - 'A' + 10;
  }
  if (character >= 'a' && character <= 'z') {
    return character - 'a' + 10;
  }
  return -1;
}

static bool scan_integer_with_base(TSLexer *lexer) {
  unsigned radix;
  if (lexer->lookahead == 'b' || lexer->lookahead == 'B') {
    radix = 2;
    lexer->advance(lexer, false);
  } else if (lexer->lookahead == 'o' || lexer->lookahead == 'O') {
    radix = 8;
    lexer->advance(lexer, false);
  } else if (lexer->lookahead == 'x' || lexer->lookahead == 'X') {
    radix = 16;
    lexer->advance(lexer, false);
  } else {
    radix = 0;
    unsigned radix_digits = 0;
    while (radix_digits < 2 && lexer->lookahead >= '0' &&
           lexer->lookahead <= '9') {
      radix = radix * 10 + lexer->lookahead - '0';
      lexer->advance(lexer, false);
      radix_digits++;
    }
    if (radix_digits == 0 ||
        (lexer->lookahead != 'r' && lexer->lookahead != 'R')) {
      return false;
    }
    lexer->advance(lexer, false);
  }

  if (radix < 2 || radix > 36) {
    return false;
  }
  if (lexer->lookahead == '+' || lexer->lookahead == '-') {
    lexer->advance(lexer, false);
  }

  bool has_digit = false;
  bool valid = true;
  int value;
  while ((value = digit_value(lexer->lookahead)) >= 0) {
    has_digit = true;
    valid = valid && (unsigned)value < radix;
    lexer->advance(lexer, false);
  }

  if (!has_digit || !valid) {
    return false;
  }

  lexer->result_symbol = INTEGER_WITH_BASE;
  return true;
}

bool tree_sitter_elisp_external_scanner_scan(void *payload, TSLexer *lexer,
                                             const bool *valid_symbols) {
  (void)payload;

  if (!valid_symbols[INTEGER_WITH_BASE]) {
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
  return scan_integer_with_base(lexer);
}
