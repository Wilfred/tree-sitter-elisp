# v1.7.0 (unreleased)

Added support for uninterned symbols, such as `#:foo`. These were
previously parse errors.

Floats with signed exponents, such as `1e-5`, are now parsed as floats
rather than symbols.

`+1.0e+INF` and `+0.0e+NaN` are now parsed as floats, and symbols that
merely resemble them, such as `1x0e+INF`, are no longer parsed as
floats.

Integers written with a radix, such as `#x2603`, are now parsed as a
single integer. Previously only the first digit was included, so the
rest became a second literal.

Radix integers may now be written with an uppercase prefix, such as
`#XF6` or `#24R1k`, and may be negative, such as `#x-8000`. These were
previously parse errors.

Added support for records, such as `#s(foo 1 2)`, as a new
`record` node. These were previously parse errors. `#s(hash-table ...)`
is still parsed as a `hash_table`, but a record whose type merely starts
with those characters, such as `#s(hash-table-p 1)`, is no longer
mistaken for a hash table.

Added support for circular and shared structure read syntax, such as
`'#1=(a . #1#)`, as new `circular_definition` and `circular_reference`
nodes. These were previously parse errors.

Added support for bool vectors, such as `#&8"\0"`, as a new
`bool_vector` node. These were previously parse errors.

Added support for symbols that bypass shorthands, such as `#_foo`. These
were previously parse errors.

Character literals whose final character is escaped, such as `?\C-\[`
and `?\^\\`, are now parsed correctly. Previously the trailing backslash
consumed the following character, so `?\C-\[` opened a vector and
swallowed the rest of the file.

Character modifiers may now be applied to any escape sequence, so
`?\C-\0`, `?\M-\x178` and `?\M-\N{SNOWMAN}` are each parsed as a single
character. Previously only the modifier prefix was included, and the
escape sequence became a separate literal.

# v1.6.1 (released 15 November 2025)

Updated Rust bindings to use tree-sitter-language.

# v1.6.0 (released 15 November 2025)

Updated tree-sitter version, rebuild parser, and added
tree-sitter.json.

# v1.5.0 (released 18 June 2024)

Updated tree-sitter version and rebuilt parser.

# v1.4.0 (released 18 June 2024)

No functional changes, exercising the release process.

# v1.3 (released 3 June 2023)

Don't error on quoted forms that look like function definitions (such
as `'(defun foo)`).

# v1.2

Added some basic syntax highlighting support ("queries" in tree-sitter
terms).

Function definitions are now handled separately from other
s-expressions. Added highlighting and tags table queries for function
definitions.

Macros are also handled separately to other s-expressions. They are
treated the same as functions for highlighting and tags tables.

Special forms are now parsed and highlighted separately from
s-expressions.

Added highlighting for `nil` and `t`.

# v1.1

Added support for more special read syntax.

Added support for bytecode literals.

Linefeed characters (commonly used as section delimiters) are now treated
as whitespace rather than parse errors.

Fixed handling of string literals with newline escaping:

```
"foo\
bar"
```

Fixed handling escaped characters and non-ASCII character in symbols.

# v1.0

Initial release.
