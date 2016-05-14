%{
#include <iostream>
#include <cstdio>
#include <list>

extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;
extern "C" int mylineno;

void yyerror(const char *msg);
%}

%code requires {
#include "small_ast.h"
}

%code {
// The root of the AST
Statement *ast_root;

// tmp Expr list for building list literals and tuples
// TODO: Got to be a safer way to do this. Consider nested lists!
std::list<Expr *> tmp_list;
std::list<char*> tmp_args;
}

%define parse.error verbose

%union{
    int ival;
    float fval;
    char *id;
    char *strlit;
    char charlit;
    bool boollit;
    Op2 op2;
    Op1 op1;
    Expr *expr;
    Statement *stateval;
}

%token <ival> INT
%token <fval> FLOAT
%token <id> ID
%token <strlit> STRING
%token <charlit> CHAR
%token <boollit> BOOL

%right <op1> LNOT
%precedence NEG
%left <op2> MUL DIV MOD
%left <op2> ADD SUB

%left <op2> LT LTE GT GTE
%left <op2> EQ

%left <op2> LAND LOR

%token LAMBDA_OPEN "(\\"
%token LAMBDA_ARROW "->"
%token FUNC "func"
%token LINE_COMMENT "//"

%token ENDL
%token RETURN

%type <expr> expr list tuple lambda
%type <stateval> program stmt seq any func_body

%token END 0 "end of file"
%%

program:
    ENDLS seq   { $$ = $2; ast_root = $$; }
    | seq       { $$ = $1; ast_root = $$;}

seq:
   stmt ENDLS seq  { $$ = new Seq($1, $3); }
   | stmt ENDLS    { $$ = $1; }

stmt:
    ID '=' expr   { $$ = new Assign($1, $3); }
    | FUNC ID[name] id_list '=' '{' func_body[body] '}'
        { $$ = new Assign($name, new ELambda(tmp_args, $body)); tmp_args.clear(); }
    | FUNC ID[name] '=' '{' func_body[body] '}'
        { $$ = new Assign($name, new ELambda(tmp_args, $body)); tmp_args.clear(); }
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
    | lambda { $$ = $1; }
    | expr ADD expr { $$ = new EOp2(Op2::Add, $1, $3); }
    | expr SUB expr { $$ = new EOp2(Op2::Sub, $1, $3); }
    | expr MUL expr { $$ = new EOp2(Op2::Mul, $1, $3); }
    | expr DIV expr { $$ = new EOp2(Op2::Div, $1, $3); }
    | expr MOD expr { $$ = new EOp2(Op2::Mod, $1, $3); }
    | expr LAND expr { $$ = new EOp2(Op2::LAnd, $1, $3); }
    | expr LOR expr { $$ = new EOp2(Op2::LOr, $1, $3); }
    | expr LT expr { $$ = new EOp2(Op2::Lt, $1, $3); }
    | expr LTE expr { $$ = new EOp2(Op2::Lte, $1, $3); }
    | expr GT expr { $$ = new EOp2(Op2::Gt, $1, $3); }
    | expr GTE expr { $$ = new EOp2(Op2::Gte, $1, $3); }
    | expr EQ expr { $$ = new EOp2(Op2::Eq, $1, $3); }
    | LNOT expr      { $$ = new EOp1(Op1::LNot, $2); }
    | SUB expr %prec NEG { $$ = new EOp1(Op1::Neg, $2); }
    /* | '(' expr ')' { $$ = $2; } */

comma_sep:
    expr    { tmp_list.push_back($1); }
    | comma_sep ',' expr { tmp_list.push_back($3); }
    | %empty

list:
    '[' comma_sep ']' { $$ = new EList(tmp_list); tmp_list.clear(); }

tuple:
     '(' comma_sep ')' { $$ = new ETuple(tmp_list); tmp_list.clear(); }

id_list:
       ID           { tmp_args.push_back($1); }
       | id_list ID { tmp_args.push_back($2); }

func_body:
         expr  { $$ = new Return($1); }
         | seq { $$ = $1; }

lambda:
      LAMBDA_OPEN id_list LAMBDA_ARROW func_body ')'
       { $$ = new ELambda(tmp_args, $4); tmp_args.clear(); }
      | LAMBDA_OPEN LAMBDA_ARROW func_body ')'
       { $$ = new ELambda(tmp_args, $3); tmp_args.clear(); }

ENDLS:
     ENDL
     | ENDLS ENDL

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
    std::cout << "Parse error on line " << mylineno << ": " << msg << '\n';
}
