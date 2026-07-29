const COMMENT = token(/;.*/);

const STRING = token(
  seq('"', repeat(choice(/[^"\\]/, seq("\\", /(.|\n)/))), '"')
);

// Symbols can contain any character when escaped:
// https://www.gnu.org/software/emacs/manual/html_node/elisp/Symbol-Type.html
// Most characters do not need escaping, but space and parentheses
// certainly do.
//
// Symbols also cannot start with ?.
const SYMBOL = token(
  choice(
    /([^.?# \n\s\f()\[\]'`,\\";]|\\.)([^# \n\s\f()\[\]'`,\\";]|\\.)*/,
    /\.([^# \n\s\f()\[\]'`,\\";]|\\.)+/
  )
);

const ESCAPED_READER_SYMBOL = token(/\\(`|'|,)/);
const INTERNED_EMPTY_STRING = token("##");

// Uninterned symbols are written with a #: prefix, e.g. #:foo. The name
// may be empty, so #: on its own is also a symbol.
// https://www.gnu.org/software/emacs/manual/html_node/elisp/Creating-Symbols.html
const UNINTERNED_SYMBOL = token(/#:([^# \n\s\f()\[\]'`,\\";]|\\.)*/);

const INTEGER_BASE10 = token(/[+-]?[0-9]+\.?/);
// The radix prefix may be written in either case, e.g. #XF6 and #24R1k
// are valid. The digits may be preceded by a sign, e.g. #x-8000.
// https://www.gnu.org/software/emacs/manual/html_node/elisp/Integer-Basics.html
const INTEGER_WITH_BASE = token(/#([boxBOX]|[0-9][0-9]?[rR])[+-]?[0-9a-zA-Z]+/);

// Exponents may be signed, e.g. 1500000e-3.
// https://www.gnu.org/software/emacs/manual/html_node/elisp/Float-Basics.html
const EXPONENT = /[eE][+-]?[0-9]+/;

// A decimal point needs digits after it, otherwise it's an integer:
// 1. is 1, but 1.0 and .5 are floats.
const FLOAT_WITH_DEC_POINT = token(
  seq(/[+-]?[0-9]*\.[0-9]+/, optional(EXPONENT))
);
const FLOAT_WITH_EXPONENT = token(seq(/[+-]?[0-9]+\.?[0-9]*/, EXPONENT));
const FLOAT_INF = token(/[+-]?1\.0[eE]\+INF/);
const FLOAT_NAN = token(/[+-]?0\.0[eE]\+NaN/);

// Any character literal may be prefixed by modifiers, e.g. ?\C-, ?\M-
// or the equivalent ?\^. This includes the escape sequences below, so
// ?\C-\101 and ?\M-\x178 are single characters.
// https://www.gnu.org/software/emacs/manual/html_node/elisp/Other-Char-Bits.html
const CHAR_MODIFIERS = /(\\(([CMSHsA]-)|\^))*/;

const CHAR = token(/\?(\\.|.)/);
const UNICODE_NAME_CHAR = token(seq("?", CHAR_MODIFIERS, /\\N\{[^}]+\}/));
const LOWER_CODE_POINT_CHAR = token(
  seq("?", CHAR_MODIFIERS, /\\u[0-9a-fA-F]{4}/)
);
const UPPER_CODE_POINT_CHAR = token(
  seq("?", CHAR_MODIFIERS, /\\U[0-9a-fA-F]{8}/)
);
const HEX_CHAR = token(seq("?", CHAR_MODIFIERS, /\\x[0-9a-fA-F]+/));
// This also covers ?\M-\123, which previously needed its own token.
const OCTAL_CHAR = token(seq("?", CHAR_MODIFIERS, /\\[0-7]{1,3}/));

// E.g. ?\C-o or ?\^o or ?\C-\S-o. The final character may itself be
// escaped, e.g. ?\C-\[ or ?\^\\.
const KEY_CHAR = token(/\?(\\(([CMSHsA]-)|\^))+(\\.|.)/);

// https://www.gnu.org/software/emacs/manual/html_node/elisp/Special-Read-Syntax.html
const BYTE_COMPILED_FILE_NAME = token("#$");

module.exports = grammar({
  name: "elisp",

  extras: ($) => [/(\s|\f)/, $.comment],

  rules: {
    source_file: ($) => repeat($._sexp),

    _sexp: ($) =>
      choice(
        $.special_form,
        $.function_definition,
        $.macro_definition,
        $.list,
        $.vector,
        $.hash_table,
        $.bytecode,
        $.string_text_properties,
        $._atom,
        $.quote,
        $.function_quote,
        $.unquote_splice,
        $.unquote
      ),

    special_form: ($) =>
      seq(
        "(",
        choice(
          "and",
          "catch",
          "cond",
          "condition-case",
          "defconst",
          "defvar",
          "function",
          "if",
          "interactive",
          "lambda",
          "let",
          "let*",
          "or",
          "prog1",
          "prog2",
          "progn",
          "quote",
          "save-current-buffer",
          "save-excursion",
          "save-restriction",
          "setq",
          "setq-default",
          "unwind-protect",
          "while"
        ),
        choice(
          repeat($._sexp),
          seq(repeat($._sexp), $.dot, $._sexp),
          seq(repeat($._sexp), alias($.dot, $.symbol))
        ),
        ")"
      ),

    function_definition: ($) =>
      prec(
        1,
        seq(
          "(",
          choice("defun", "defsubst"),
          field("name", $.symbol),
          optional(field("parameters", $._sexp)),
          optional(field("docstring", $.string)),
          repeat($._sexp),
          ")"
        )
      ),

    macro_definition: ($) =>
      prec(
        1,
        seq(
          "(",
          "defmacro",
          field("name", $.symbol),
          optional(field("parameters", $._sexp)),
          optional(field("docstring", $.string)),
          repeat($._sexp),
          ")"
        )
      ),

    _atom: ($) =>
      choice(
        $.float,
        $.integer,
        $.char,
        $.string,
        $.byte_compiled_file_name,
        $.symbol
      ),
    float: ($) =>
      choice(FLOAT_WITH_DEC_POINT, FLOAT_WITH_EXPONENT, FLOAT_INF, FLOAT_NAN),
    integer: ($) => choice(INTEGER_BASE10, INTEGER_WITH_BASE),
    char: ($) =>
      choice(
        CHAR,
        UNICODE_NAME_CHAR,
        LOWER_CODE_POINT_CHAR,
        UPPER_CODE_POINT_CHAR,
        HEX_CHAR,
        OCTAL_CHAR,
        KEY_CHAR
      ),
    string: ($) => STRING,
    byte_compiled_file_name: ($) => BYTE_COMPILED_FILE_NAME,
    symbol: ($) =>
      choice(
        // Match nil and t separately so we can highlight them.
        "nil",
        "t",
        // We need to define these as separate tokens so we can handle
        // e.g '(defun) as a sexp. Without these, we just try
        // function_definition and produce a parse failure.
        "defun",
        "defsubst",
        "defmacro",
        ESCAPED_READER_SYMBOL,
        SYMBOL,
        INTERNED_EMPTY_STRING,
        UNINTERNED_SYMBOL
      ),

    quote: ($) => seq(choice("'", "`"), $._sexp),

    // Sharp quote reads as (function ...) rather than (quote ...), so
    // the quoted form is a function rather than a literal.
    // https://www.gnu.org/software/emacs/manual/html_node/elisp/Anonymous-Functions.html
    function_quote: ($) => seq("#'", $._sexp),

    unquote_splice: ($) => seq(",@", $._sexp),
    unquote: ($) => seq(",", $._sexp),

    dot: ($) => token("."),
    list: ($) =>
      seq(
        "(",
        choice(
          repeat($._sexp),
          seq(repeat1($._sexp), $.dot, $._sexp),
          seq(repeat1($._sexp), alias($.dot, $.symbol))
        ),
        ")"
      ),
    vector: ($) => seq("[", repeat($._sexp), "]"),
    bytecode: ($) => seq("#[", repeat($._sexp), "]"),

    string_text_properties: ($) => seq("#(", $.string, repeat($._sexp), ")"),

    hash_table: ($) => seq("#s(hash-table", repeat($._sexp), ")"),

    comment: ($) => COMMENT,
  },
});
