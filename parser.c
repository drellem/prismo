#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"

#define DEBUG_PARSE 1

// Parse functions return NULL if they fail

int parse_stmt(TokenList**, Ast*);
int precedence(Ast*);
int left_assoc(Ast*);

Ast* parse(TokenList* tokens, Ast* head){
  TokenList** current_pos = &tokens;
  TokenList** current_pos_copy = current_pos;

  printf("\nFirst token is %s\n", token_str((*current_pos)->val));
  fflush(stdout);

  Token* curr = tokens->val;

  if(curr==NULL || curr->type == TOKEN_EOF){
    return NULL;
  }
  
  if (head==NULL){
    if(!(head = malloc(sizeof(Ast)))){
          printf("\nError allocating memory\n");
          fflush(stdout);
          return NULL;

    }
    head->type = AST_HEAD;
    head->meta = "HEAD";
    head->children = astEmptyList();
  }

  for(;;){
    if((*current_pos)->val->type == TOKEN_EOF || !parse_stmt(current_pos, head)){
      current_pos = current_pos_copy;
      if(DEBUG_PARSE){
        printf("\nFailed statement parse\n");
        fflush(stdout);
      }
      break;
    } else {
      if(DEBUG_PARSE){
        printf("\nSuccessful statement parse\n");
        fflush(stdout);
        printf("\nNow we are in position %s\n", token_str((*current_pos)->val));
        fflush(stdout);
      }

      current_pos_copy = current_pos;
    } 
  }

  if(head->children==NULL){
    printf("\nWarning there are no statements!\n");
    fflush(stdout);
  }
  return head;
}

int parse_stmt(TokenList** current_pos, Ast* head){
  Ast* result;
  AstStack* operands = createStack();
  AstStack* operators = createStack();

  if( !(result = malloc(sizeof(Ast)))){
    printf("\nError allocating memory\n");
    fflush(stdout);
    return 0;
  }
  

  if((*current_pos)==NULL) {
    result->type = AST_EOF;
    printf("\nUnexpected EOF end of token stream.\n");
    fflush(stdout);
    return 0;
  }

  Token* curr = (*current_pos)->val;


  for(;;){
    printf("\nStatement token %s\n", token_str(curr));
    fflush(stdout);
    
    if(curr==NULL){
      printf("\nReached end of file\n");
      fflush(stdout);

      printf("\nToken is %s\n", token_str(curr));
      fflush(stdout);
      return 0;
    }
      
    switch(curr->type){
    case TOKEN_NUM:
      push(operands, ast_num(curr->meta));
      break;
    case TOKEN_ID:
      push(operands, ast_id(curr->meta));
      break;
    case TOKEN_BINOP:
    TOKEN_BINOP_LBL:
      {

        Ast* ast_curr = ast_binop(curr->meta);
        int a = precedence(ast_curr);
        int b = precedence(peek(operators));
        printf("\nCurrent operator is %s\n", ast_curr->meta);
        printf("\nPrecedences are a %d and b %d\n", a, b);
        fflush(stdout);
        if( b > a || (b == a && left_assoc(peek(operators)))){

          Ast* node1 = pop(operands);
          Ast* node2 = pop(operands);
          if( node1 == NULL || node2 == NULL ){
            printf("\nError parsing statement. Mismatched operators and operands.\n");
            return 0;
          }
          Ast* op = pop(operators);
          op->children = astAppendToHead(op->children, node1);
          op->children = astAppendToHead(op->children, node2);
          push(operands, op);
          goto TOKEN_BINOP_LBL;
        } else {
          push(operators, ast_curr);
        }
      }
    case TOKEN_EOF:
      break;
    }
    if(curr->type==TOKEN_EOF){
      break;
    }
    *current_pos = (*current_pos)->next;
    curr = (*current_pos)->val;
  }

  printf("\nAbout to commence popping remaining operators\n");
  fflush(stdout);

  Ast* ast_curr = pop(operators);

  while(ast_curr != NULL){

    printf("\nParsing remaining operator %s\n", ast_curr->meta);
    fflush(stdout);
    
    Ast* node1 = pop(operands);
    Ast* node2 = pop(operands);
    if(node1 == NULL || node2 == NULL){
      printf("\nError parsing statement. Mismatched operators and operands.\n");
      fflush(stdout);
      return 0;
    }
    ast_curr->children = astAppendToHead(ast_curr->children, node1);
    ast_curr->children = astAppendToHead(ast_curr->children, node2);
    push(operands, ast_curr);
    ast_curr = pop(operators);
  }
  result = pop(operands);
  Ast* result2 = pop(operands);
  if( result2 != NULL ){
    printf("\nError, operands left after statement was parsed\n");
    fflush(stdout);
    return 0;
  }

  printf("\nAbout to append the result\n");
  fflush(stdout);
  
  astAppendToTail(head->children, result);

  printf("\nAbout to free the data\n");
  fflush(stdout);
  //Now clean up any operators left in the stack
  free(operands);
  free(operators);

  printf("\nSuccessfully parsed the statement\n");
  fflush(stdout);
  
  return 1;
}

int precedence(Ast* binop){
  if(binop==NULL){
    return 0;
  }
  if(!strcmp(binop->meta, "+") || !strcmp(binop->meta, "-")){
    return 1;
  }
  if(!strcmp(binop->meta, "*") || !strcmp(binop->meta, "/")){
    return 2;
  }
}

int left_assoc(Ast* binop){
  if(binop==NULL){
    printf("\nError: Unexpected NULL binop.\n");
    return 1;
  }
  if(!strcmp(binop->meta, "^")){
    return 0;
  }
  return 1;
}
