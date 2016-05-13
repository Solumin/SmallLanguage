%{
#include <iostream>
#include <cstdio>
#include <list>

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

// tmp Expr list for building list literals and tuples
std::list<Expr *> tmp_list;
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

%token ENDL
%token RETURN

%type <expr> expr list tuple
%type <stateval> program stmt seq any

%token END 0 "end of file"
%%

program:
    seq     { $$ = $1; ast_root = $$; }

seq:
   stmt ENDLS seq  { $$ = new Seq($1, $3); }
   | stmt ENDLS    { $$ = $1; }

stmt:
    ID '=' expr   { $$ = new Assign($1, $3); }
    | RETURN expr { $$ = new Return($2); }
    | any   { $$ = $1; }

any:
   expr     { $$ = new Any($1); }

expr:
    INT     { $$ = new EInt($1); }
    | FLOAT  { $$ = new EFloat($1); }
    | ID     { $$ = new EId($1); }
    | STRING { $$ = new EString($1); }
    | CHAR   { $$ = new EChar($1); }
    | BOOL   { $$ = new EBool($1); }
    | list   { $$ = $1; }
    | tuple  { $$ = $1; }

comma_sep:
    expr    { tmp_list.push_back($1); }
    | comma_sep ',' expr { tmp_list.push_back($3); }

list:
    '[' comma_sep ']' { $$ = new EList(tmp_list); tmp_list.clear(); }

tuple:
     '(' comma_sep ')' { $$ = new ETuple(tmp_list); tmp_list.clear(); }

ENDLS:
     ENDLS ENDL
    | ENDL

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
