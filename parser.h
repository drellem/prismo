#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

// AST* is the head value that results should be appended to
Ast* parse(TokenList*, Ast*);

#endif
