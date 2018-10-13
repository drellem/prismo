#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ast.h"


char* token_t_str(Token_t type){
  switch(type){
  case TOKEN_EOF: return "EOF";
  case TOKEN_ID: return "ID";
  case TOKEN_NUM: return "NUM";
  case TOKEN_BINOP: return "BINOP";
  case TOKEN_LP: return "(";
  case TOKEN_RP: return ")";
  case TOKEN_SC: return ";";
  case TOKEN_ASSN: return "=";
  case TOKEN_VAR: return "var";
  }
  return "";
}

char* token_str(Token* tok){
  if(tok==NULL){
    return "";
  }
  printf("\n%s: %s\n", token_t_str(tok->type), (tok->meta==NULL ? "" : tok->meta));
  fflush(stdout);

  return "";
}

TokenList* appendToHead(TokenList* head, Token* newHead){
  TokenList* ret = malloc(sizeof(TokenList));
  ret->val = newHead;
  ret->next = head;
  return ret;
}

void appendToTail(TokenList* head, Token* tail){
  if(head==NULL){
    printf("\nWarning. Appending to tail of NULL list does not work.\n");
    fflush(stdout);
    return;
  } else if(head->val == NULL){
    head->val = tail;
  } else if(head->next==NULL){
    TokenList* end = malloc(sizeof(TokenList));
    end->val = tail;
    end->next = NULL;
    head->next = end;
  } else {
    appendToTail(head->next, tail);
  }
}

TokenList* toList(Token* token){
  TokenList* ret = malloc(sizeof(TokenList));
  ret->val = token;
  ret->next = NULL;
  return ret;
}

void freeList(TokenList* head){
  if(head!=NULL){
    freeList(head->next);
    free(head->val);
    free(head);
  }
}

AstList* astAppendToHead(AstList* head, Ast* newHead){
  //  printf("\nTrying to start appending to head\n");
  fflush(stdout);
  AstList* ret;
  if( ! (ret= malloc(sizeof(AstList)))){
    printf("\nError allocating memory\n.");
    fflush(stdout);
    return NULL;
  }
  //y  printf("\nAppending to head\n");
  fflush(stdout);
  ret->val = newHead;
  ret->next = head;
  return ret;
}

void astAppendToTail(AstList* head, Ast* tail){
  if(head==NULL){
    printf("\nWarning: Appending to NULL list does not work\n");
    fflush(stdout);
  } else if(astListIsEmpty(head)){
    head->val = tail;
  } else if(head->next==NULL){
    AstList* end = malloc(sizeof(AstList));
    end->val = tail;
    end->next = NULL;
    head->next = end;
  } else {
    astAppendToTail(head->next, tail);
  }
}

AstList* astEmptyList(){
  AstList* ret = malloc(sizeof(AstList));
  ret->val = NULL;
  ret->next = NULL;
  return ret;
}

int astListIsEmpty(AstList* list){
  return list->val == NULL;
}

AstList* astToList(Ast* token){
  AstList* ret = malloc(sizeof(AstList));
  ret->val = token;
  ret->next = NULL;
  return ret;
}

void astFreeList(AstList* head){
  if(head!=NULL){
    astFreeList(head->next);
    free(head->val);
    free(head);
  }
}

void push(AstStack* stack, Ast* elem){
  //  printf("\nPushing %s", elem->meta);
  //  if(peek(stack)!=NULL)printf(" onto %s %s", peek(stack)->meta, token_t_str(peek(stack)->type));
  printf("\n");
  fflush(stdout);
  stack->list = astAppendToHead(stack->list, elem);
  //  printf("\nThe push is done\n.");
  fflush(stdout);
}

Ast* pop(AstStack* stack){
  if(stack==NULL || stack->list==NULL){
    return NULL;
  } else {
    Ast* tmp = stack->list->val;
    stack->list = stack->list->next;
    if(tmp!=NULL){
      //      printf("\nPopping %s\n", tmp->meta);
      fflush(stdout);
    }
    return tmp;
  }      
}

Ast* peek(AstStack* stack){
  if(stack->list==NULL){
    return NULL;
  } else {
    return stack->list->val;
  }
}

AstStack* createStack(){
  AstStack* stack;
  if( ! (stack = malloc(sizeof(AstStack)))){
    printf("\nError allocating memory\n");
    fflush(stdout);
    return NULL;
  }
  stack->list = NULL;
  return stack;
}


/*
  These methods make Ast leaves out of values
 */
Ast* ast_num(char* meta){
  Ast* ast = malloc(sizeof(Ast));
  ast->type = AST_NUM;
  ast->meta = meta;
  ast->children = NULL;
  return ast;
}

Ast* ast_id(char* meta){
  Ast* ast = malloc(sizeof(Ast));
  ast->type = AST_ID;
  ast->meta = meta;
  ast->children = NULL;
  return ast;
}

Ast* ast_binop(char* meta){
  Ast* ast = malloc(sizeof(Ast));
  ast->type = AST_BINOP;
  ast->meta = meta;
  ast->children = NULL;
  return ast;
}

Ast* ast_lp(){
  Ast* ast = malloc(sizeof(Ast));
  ast->type = AST_LP;
  ast->meta = "(";
  ast->children = NULL;
  return ast;
}

char* ast_t_str(Ast_t type){
  switch(type){
  case AST_HEAD:
      return "HEAD";
  case AST_EOF:
    return "EOF";
  case AST_NUM:
    return "NUM";
  case AST_ID:
    return "ID";
  case AST_BINOP:
    return "BINOP";
  case AST_LP:
    return "(";
  case AST_VAR:
    return "VAR";
  default:
    return "";
  }
}
  

void printAst(Ast* ast, int depth){
  int i;
  for(i =  0; i < depth; i++){
    printf("\t");
  }
  if(ast->meta==NULL)
    printf("No meta\n");
  else
    printf("%s:%s\n", ast_t_str(ast->type), ast->meta);
  fflush(stdout);
  AstList* child = ast->children;
  while(child!=NULL && child->val!=NULL){
    fflush(stdout);
    printAst(child->val, depth+1);
    child = child->next;
  }
}


void printAstStack(AstStack* stack){
  AstStack* stack2 = createStack();
  Ast* elem = pop(stack);
  //  printf("\nSuccessfully popped\n");
  fflush(stdout);
  int depth;
  for(depth = 0; elem!=NULL; depth++){
    push(stack2, elem);
    elem = pop(stack);
  }
  //  printf("\nFinished with depth %d\n", depth);
  fflush(stdout);
  int i;
  for(i = 0; i < depth; i++){
    elem = pop(stack2);
    printAst(elem, 0);
    fflush(stdout);
    if(elem!=NULL)
      push(stack, elem);
  }
  freeStack(stack2);
  printf("\nDone printing stack\n");
  fflush(stdout);
}

void freeStack(AstStack* stack){
  free(stack->list);
  free(stack);
}

void printTokenList(TokenList* list){
  while(list!=NULL){
    token_str(list->val);
    list = list->next;
  }
}
