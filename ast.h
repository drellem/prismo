#ifndef AST_H
#define AST_H

#define TOKEN_META_MAX_LEN 51

enum Ast_t_enum { AST_HEAD, AST_EOF, AST_NUM, AST_ID, AST_BINOP,
                  AST_LP };

typedef enum Ast_t_enum Ast_t;

typedef struct Asts {
  Ast_t type;
  char* meta;
  struct AstListStruct* children;
} Ast;

Ast* ast_num(char*);
Ast* ast_id(char*);
Ast* ast_binop(char*);
Ast* ast_lp();

enum Token_t_enum { TOKEN_EOF, TOKEN_ID, TOKEN_NUM, TOKEN_BINOP,
                    TOKEN_LP,  TOKEN_RP };

typedef enum Token_t_enum Token_t;
  
typedef struct  {
  Token_t type;
  char meta[TOKEN_META_MAX_LEN];
} Token;


char* token_t_str(Token_t);

char* token_str(Token*);

typedef struct TokenListStruct {
  Token* val;
  struct TokenListStruct* next;
} TokenList;

TokenList* appendToHead(TokenList*, Token*);
void appendToTail(TokenList*, Token*);
TokenList* toList(Token*);

void freeList(TokenList*);


typedef struct AstListStruct {
  Ast* val;
  struct AstListStruct* next;
} AstList;

AstList* astAppendToHead(AstList*, Ast*);

void astAppendToTail(AstList*, Ast*);
AstList* astToList(Ast*);
void astFreeList(AstList*);
AstList* astEmptyList();
int astListisEmpty();
void printAstList(AstList*);

typedef struct AstStack {
  AstList* list;
} AstStack;

void push(AstStack*, Ast*);
Ast* pop(AstStack*);
Ast* peek(AstStack*);
AstStack* createStack();
void printAstStack(AstStack*);
void freeStack(AstStack*);


void printAst(Ast*, int);

void printTokenList(TokenList*);
#endif
