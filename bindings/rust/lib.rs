//! This crate provides Emacs Lisp language support for the [tree-sitter] parsing library.
//!
//! Typically, you will use the [`LANGUAGE`] constant to add this language to a
//! tree-sitter [`Parser`], and then use the parser to parse some code:
//!
//! ```
//! let code = r#"
//! "#;
//! let mut parser = tree_sitter::Parser::new();
//! let language = tree_sitter_elisp::LANGUAGE;
//! parser
//!     .set_language(&language.into())
//!     .expect("Error loading Emacs Lisp parser");
//! let tree = parser.parse(code, None).unwrap();
//! assert!(!tree.root_node().has_error());
//! ```
//!
//! [`Parser`]: https://docs.rs/tree-sitter/0.25.10/tree_sitter/struct.Parser.html
//! [tree-sitter]: https://tree-sitter.github.io/

use tree_sitter_language::LanguageFn;

extern "C" {
    fn tree_sitter_elisp() -> *const ();
}

/// The tree-sitter [`LanguageFn`] for this grammar.
pub const LANGUAGE: LanguageFn = unsafe { LanguageFn::from_raw(tree_sitter_elisp) };

/// The content of the [`node-types.json`] file for this grammar.
///
/// [`node-types.json`]: https://tree-sitter.github.io/tree-sitter/using-parsers/6-static-node-types
pub const NODE_TYPES: &str = include_str!("../../src/node-types.json");

// NOTE: uncomment these to include any queries that this grammar contains:

pub const HIGHLIGHTS_QUERY: &str = include_str!("../../queries/highlights.scm");
// pub const INJECTIONS_QUERY: &str = include_str!("../../queries/injections.scm");
// pub const LOCALS_QUERY: &str = include_str!("../../queries/locals.scm");
pub const TAGS_QUERY: &str = include_str!("../../queries/tags.scm");

#[cfg(test)]
mod tests {
    #[test]
    fn test_can_load_grammar() {
        let mut parser = tree_sitter::Parser::new();
        parser
            .set_language(&super::LANGUAGE.into())
            .expect("Error loading Emacs Lisp parser");
    }

    #[test]
    fn test_string_with_nul_bytes() {
        let mut parser = tree_sitter::Parser::new();
        parser
            .set_language(&super::LANGUAGE.into())
            .expect("Error loading Emacs Lisp parser");
        let tree = parser.parse(b"(defconst data \"a\0b\")", None).unwrap();
        assert!(
            !tree.root_node().has_error(),
            "{}",
            tree.root_node().to_sexp()
        );
    }

    #[test]
    fn test_comment_with_nul_bytes() {
        let mut parser = tree_sitter::Parser::new();
        parser
            .set_language(&super::LANGUAGE.into())
            .expect("Error loading Emacs Lisp parser");
        let tree = parser.parse(b"; a\0b\nfoo", None).unwrap();
        assert!(
            !tree.root_node().has_error(),
            "{}",
            tree.root_node().to_sexp()
        );
    }

    #[test]
    fn test_raw_nul_and_newline_characters() {
        let mut parser = tree_sitter::Parser::new();
        parser
            .set_language(&super::LANGUAGE.into())
            .expect("Error loading Emacs Lisp parser");

        for code in [b"?\0".as_slice(), b"?\n", b"?\\\0", b"?\\\n"] {
            let tree = parser.parse(code, None).unwrap();
            assert!(
                !tree.root_node().has_error(),
                "{:?}: {}",
                code,
                tree.root_node().to_sexp()
            );
        }
    }

    #[test]
    fn test_raw_escaped_symbol_characters() {
        let mut parser = tree_sitter::Parser::new();
        parser
            .set_language(&super::LANGUAGE.into())
            .expect("Error loading Emacs Lisp parser");

        for code in [
            b"foo\\\0bar".as_slice(),
            b"foo\\\nbar",
            b"#:foo\\\0bar",
            b"#:foo\\\nbar",
        ] {
            let tree = parser.parse(code, None).unwrap();
            assert!(
                !tree.root_node().has_error(),
                "{:?}: {}",
                code,
                tree.root_node().to_sexp()
            );
        }
    }

    #[test]
    fn test_bytecode_comments() {
        let mut parser = tree_sitter::Parser::new();
        parser
            .set_language(&super::LANGUAGE.into())
            .expect("Error loading Emacs Lisp parser");
        let tree = parser.parse(b"#@5a\0b\ncfoo", None).unwrap();
        assert!(
            !tree.root_node().has_error(),
            "{}",
            tree.root_node().to_sexp()
        );
    }

    #[test]
    fn test_radix_integer_validation() {
        let mut parser = tree_sitter::Parser::new();
        parser
            .set_language(&super::LANGUAGE.into())
            .expect("Error loading Emacs Lisp parser");

        for code in [
            b"#b2".as_slice(),
            b"#o8",
            b"#xg",
            b"#2r2",
            b"#37rz",
            b"#b10foo",
        ] {
            let tree = parser.parse(code, None).unwrap();
            assert!(
                tree.root_node().has_error(),
                "{:?} parsed without errors",
                code
            );
        }
    }
}
