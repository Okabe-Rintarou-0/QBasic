#include "syntax.h"

namespace syntax {
    SyntaxTree::SyntaxTree(Exp *root) : root(root) {}

    SyntaxTree::~SyntaxTree() { clear(); }
}