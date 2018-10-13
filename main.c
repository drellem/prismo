#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "ast.h"


int main(){

  char* prog = "3*(2+1)-5;2+7;";
  
  TokenList* tok_list = lex(prog);

  printf("\nToken list is %s\n", token_str(tok_list->val));
  fflush(stdout);
                                                             

  Ast* ast = parse(tok_list, ((Ast*)NULL));

  printAst(ast, 0);

  

  /*  Token* id = malloc(sizeof(Token));                               
  id-> type = TOKEN_ID;
  strcpy(id->meta, "APPLE"); */

  

  //  TokenList* tok_list = appendToHead(NULL, id);

  printf("\nSuccessfully parsed\n");
  fflush(stdout);
    
  freeList(tok_list);
}
