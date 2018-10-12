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
  }
  return "";
}

char* token_str(Token* tok){
  char* type_str = token_t_str(tok->type);
  char* delim = ": ";
  char* ret;

  if(!(ret=malloc(sizeof(char)*(strlen(type_str)+strlen(delim)+strlen(tok->meta)+1)))){
    printf("Error allocating memory")           ;
    return NULL;
  }
 
  strcpy(ret, type_str);

  strcat(ret, delim);

  strcat(ret, tok->meta);

  return ret;
}

TokenList* appendToHead(TokenList* head, Token* newHead){
  TokenList* ret = malloc(sizeof(TokenList));
  ret->val = newHead;
  ret->next = head;
  return ret;
}

void appendToTail(TokenList* head, Token* tail){
  if(head==NULL){
    head = malloc(sizeof(TokenList));
    head->val = tail;
    head->next = NULL;
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
  AstList* ret = malloc(sizeof(AstList));
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
  printf("\nPushing %s", elem->meta);
  if(peek(stack)!=NULL)printf(" onto %s", peek(stack)->meta);
  printf("\n");
  fflush(stdout);
  stack->list = astAppendToHead(stack->list, elem);
}

Ast* pop(AstStack* stack){
  if(stack==NULL || stack->list==NULL){
    return NULL;
  } else {
    Ast* tmp = stack->list->val;
    stack->list = stack->list->next;
    if(tmp!=NULL){
      printf("\nPopping %s\n", tmp->meta);
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
  AstStack* stack = malloc(sizeof(AstStack));
  stack->list = NULL;
  return stack;
}


/*
  These methods make Ast leaves out of values
 */
Ast* ast_num(char* meta){
  Ast* ast = malloc(sizeof(ast));
  ast->type = AST_NUM;
  ast->meta = meta;
  ast->children = NULL;
}

Ast* ast_id(char* meta){
  Ast* ast = malloc(sizeof(ast));
  ast->type = AST_ID;
  ast->meta = meta;
  ast->children = NULL;
}

Ast* ast_binop(char* meta){
  Ast* ast = malloc(sizeof(ast));
  ast->type = AST_BINOP;
  ast->meta = meta;
  ast->children = NULL;
}
