#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "ast.h"


void printAst(Ast*, int);

int main(){

  char* prog = "3-2-1";
  
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


void printAst(Ast* ast, int depth){
  int i;
  for(i =  0; i < depth; i++){
    printf("\t");
  }
  printf("%s\n", ast->meta);
  fflush(stdout);
  AstList* child = ast->children;
  while(child!=NULL && child->val!=NULL){
    fflush(stdout);
    printAst(child->val, depth+1);
    child = child->next;
  }
}

