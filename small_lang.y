%{
#include <iostream>
#include <cstdio>

extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;

void yyerror(const char *msg);
%}

%code requires {
#include "small_ast.h"
}

%code {
// The root of the AST
Statement *ast_root;
}

%define parse.error verbose

%union{
    int ival;
    float fval;
    char *id;
    char *strlit;
    char charlit;
    bool boollit;
    Expr *expr;
    Statement *stateval;
}

%token <ival> INT
%token <fval> FLOAT
%token <id> ID
%token <strlit> STRING
%token <charlit> CHAR
%token <boollit> BOOL

%type <expr> expr
%type <stateval> program seq any

%token END 0 "end of file"
%%

program:
    seq     { $$ = $1; printf("Assigning to ast_root\n"); ast_root = $$; }

seq:
   any seq  { $$ = new Seq($1, $2); }
   | any    { $$ = new Seq($1); }

any:
   expr     { $$ = new Any($1); }

expr:
    INT     { $$ = new EInt($1); }
   | FLOAT  { $$ = new EFloat($1); }
   | ID     { $$ = new EId($1); }
   | STRING { $$ = new EString($1); }
   | CHAR   { $$ = new EChar($1); }
   | BOOL   { $$ = new EBool($1); }

%%

int main( int argc, char** argv) {
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        printf("Failed to open %s\n", argv[1]);
        return 1;
    }
    while (!feof(yyin)) {
		yyparse();
    };
    printf("\nParse successful, time to print!\n");
    if (ast_root == NULL) {
        printf("Aye wtf\n");
    } else {
        std::cout << "The program:\n" << ast_root->toString() << std::endl;;
    }
    fclose(yyin);
    return 0;
}


void yyerror(const char *msg) {
    std::cout << "Parse error: " << msg << '\n';
}
