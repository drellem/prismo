#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "ast.h"


int main(){

  char* prog = "var a = 4; var alpha = 5+(7*6);";
  
  TokenList* tok_list = lex(prog);
                                                             

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
