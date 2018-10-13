#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ast.h"

#define DEBUG_LEX 1

void lex_id(char, char*, Token*, int*);
void lex_num(char, char*, Token*, int*);
int  isbinop(char);
void check_kwd(Token*);

TokenList* lex(char* c){
  TokenList* retList = NULL;
  int index = 0;
  
  for(;; index++){
    Token* ret;
    if(!(ret=malloc(sizeof(Token)))){
      printf("\nError allocating memory\n");
      return retList;
    }
  
    if(c == NULL || c[index] == '\0'){
      ret->type = TOKEN_EOF;
    } else if(isspace(c[index])){
        continue;
    } else if(isalpha(c[index])) {
      lex_id(c[index], c, ret, &index);
      check_kwd(ret);
    } else if(isdigit(c[index])){
      lex_num(c[index], c, ret, &index);
    } else if(c[index]=='('){
      ret->type = TOKEN_LP;
      ret->meta[0] = '(';
      ret->meta[1] = '\0';
    } else if(c[index]==')'){
      ret->type = TOKEN_RP;
      ret->meta[0] = ')';
      ret->meta[1] = '\0';
    } else if(c[index]==';'){
      ret->type = TOKEN_SC;
      ret->meta[0] = ';';
      ret->meta[1] = '\0';
    } else if(c[index]=='='){
      ret->type = TOKEN_ASSN;
      ret->meta[0] = '=';
      ret->meta[1] = '\0';
    } else if(isbinop(c[index])){
      ret->type = TOKEN_BINOP;
      ret->meta[0] = c[index];
      ret->meta[1] = '\0';
    }
    if(retList==NULL){
      retList = toList(ret);
    } else {
      appendToTail(retList, ret);
    }
    if(DEBUG_LEX){
      printf("\nAdding value %s\n", token_str(ret));
      printf("\nOn index %d\n", index);
      fflush(stdout);
    }
    if(ret->type == TOKEN_EOF){
      return retList;
    }
  }
  return retList;
}

void lex_num(char fst, char* in, Token* ret, int* index_ref){
  ret->type = TOKEN_NUM;
  ret->meta[0] = fst;
  int i = 1;
  char tmp;
  for(;;i++){
    tmp = in[++(*index_ref)];
    if(isdigit(tmp) && tmp!='\0'){
      if(i == (TOKEN_META_MAX_LEN-2)){
        printf("\nWarning: Identifier character limit reached in: ");
        int j;
        for(j = 1; j < i; j++){
          printf("%c", ret->meta[j]);
        }
        ret->meta[i] = '\0';
        return;
      } else {
        ret->meta[i] = tmp;
      }
    } else {
      (*index_ref)--;
      ret->meta[i] = '\0';
      return;
    }
  }
}

void lex_id(char fst, char* in, Token* ret, int* index_ref){
  ret->type = TOKEN_ID;
  ret->meta[0] = fst;
  int i = 1;
  char tmp;
  for(;;i++){
    tmp = in[++(*index_ref)];
    if((isalpha(tmp) || isdigit(tmp)) && tmp!='\0'){
      if(i == (TOKEN_META_MAX_LEN-2)){
        printf("\nWarning: Identifier character limit reached in: ");
        int j;
        for(j = 1; j < i; j++){
          printf("%c", ret->meta[j]);
        }
        ret->meta[i] = '\0';
        return;
      } else {
        ret->meta[i] = tmp;      }
    } else {
      (*index_ref)--;
      ret->meta[i] = '\0';
      return;
    }
  }
}

int isbinop(char c){
  return c=='+' || c == '-' || c == '*' || c == '/';
}

void check_kwd(Token* tok){
  if( !strcmp(tok->meta, "var") ){
    tok->type = TOKEN_VAR;
  }
}
