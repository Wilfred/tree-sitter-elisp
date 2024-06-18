package tree_sitter_elisp_test

import (
	"testing"

	tree_sitter "github.com/smacker/go-tree-sitter"
	"github.com/tree-sitter/tree-sitter-elisp"
)

func TestCanLoadGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_elisp.Language())
	if language == nil {
		t.Errorf("Error loading Elisp grammar")
	}
}
