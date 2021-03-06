#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"

#define DEBUG_PARSE 1

// Parse functions return NULL if they fail

int parse_stmt(TokenList**, Ast*);
int parse_val(TokenList**, Ast*);
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
  if((*current_pos)==NULL){
    printf("\nUnexpected EOF end of token stream.\n");
    fflush(stdout);
    return 0;
  }

  Token* curr = (*current_pos)->val;

  switch(curr->type){
  case TOKEN_VAR:
    *current_pos = (*current_pos)->next;
    return parse_var(current_pos, head);
  case TOKEN_SC:
    *current_pos = (*current_pos)->next;
    return 1;
  case EOF:
    return 0;
  default:
    printf("\nError parsing statement. Unrecognized token:\n");
    token_str(curr);
    return 0;
  }
  
}

int parse_var(TokenList** current_pos, Ast* head){
  Ast* result;

  if((*current_pos)==NULL){
    printf("\nError unexpected EOF end of token stream while parsing 'var'.\n");
    fflush(stdout);
    return 0;
  }

  if(!(result = malloc(sizeof(Ast)))){
    printf("\nError allocating memory\n");
    return 0;
  }

  result->type = AST_VAR;
  result->children = astEmptyList();

  Token* var_name_token = (*current_pos)->val;

  if(var_name_token->type != TOKEN_ID){
    printf("\nError parsing var. Expected var ID but found token:");
    token_str(var_name_token);
    return 0;
  }

  result->meta = var_name_token->meta;

  *current_pos = (*current_pos)->next;

  Token* eq_token = (*current_pos)->val;

  if(eq_token==NULL){
    if((*current_pos)==NULL){
      printf("\nError unexpected EOF end of token stream while parsing 'var'.\n");
      fflush(stdout);
      return 0;
    }
  }

  if(eq_token->type!=TOKEN_ASSN){
    printf("\nError, expected token '=' in var ID '=' ... but found token:");
    token_str(eq_token);
  }

  *current_pos = (*current_pos)->next;

  int success = parse_expr(current_pos, result);

  if(success){
    astAppendToTail(head->children, result);
  }
  
  return success;
  
}


/*
  This looks heinous but it's just the Shunting Yard algorithm.

  The code is designed procedurally so that each parsing substep updates the current
  position in the token stream. The position is reset upon failure of a substep.
 */

int parse_expr(TokenList** current_pos, Ast* head){
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

  int stop = 0;
  
  while( !stop ) {
    printf("\nStatement token: ");
    token_str(curr);
    fflush(stdout);

    printf("\nOperands:\n");
    printAstStack(operands);

    printf("\nOperators:\n");
    printAstStack(operators);

    printf("\nToken List;\n");
    printTokenList(*current_pos);
    
    if(curr==NULL){
      printf("\nReached end of file\n");
      fflush(stdout);

      printf("\nToken is\n");
      token_str(curr);
     
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
      break;
      
    case TOKEN_LP:
      push(operators, ast_lp());

      printf("\nSuccessfully pushed the LP\n");
      break;
        
    case TOKEN_RP:
      { 
        Ast* curr_op = pop(operators);
        while(curr_op!=NULL && curr_op->type != AST_LP){
          printf("\nBeginning with the operator %s\n", curr_op->meta);
          fflush(stdout);
          Ast* node1 = pop(operands);
          Ast* node2 = pop(operands);
          if(node1!=NULL)
            printf("\nUsing node1 %s", node1->meta);
          fflush(stdout);
          if(node2!=NULL)
            printf("\nUsing node2 %s", node2->meta);
          fflush(stdout);
          if( node1==NULL || node2==NULL ){
            printf("\nError parsing statement. Mismatched operators and operands in parentheses.\n");
            fflush(stdout);
            return 0;
          }
          curr_op->children = astAppendToHead(curr_op->children, node1);
          curr_op->children = astAppendToHead(curr_op->children, node2);
          push(operands, curr_op);
          curr_op = pop(operators);
        }
        if( curr_op==NULL ){
          printf("\nError parsing statement. Mismatched parentheses.\n");
          fflush(stdout);
          return 0;
        }
        printf("\nFinished parsing parentheses\n");
        fflush(stdout);
      }
      break;

    default:
      stop = 1;
      break;
    }

    // If we stopped parsing because we hit a semicolon, skip it
    if(!stop || stop && (curr->type == TOKEN_SC)){
      *current_pos = (*current_pos)->next;
      curr = (*current_pos)->val;
    }    
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

int precedence(Ast* op){
  if(op==NULL || !strcmp(op->meta, "(")){
    return 0;
  }
  if(!strcmp(op->meta, "+") || !strcmp(op->meta, "-")){
    return 1;
  }
  if(!strcmp(op->meta, "*") || !strcmp(op->meta, "/")){
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
