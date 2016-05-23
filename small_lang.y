%{
#include <iostream>
#include <vector>

extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;

void yyerror(const char *msg);
%}

%code requires {
#include "small_expr.hpp"
#include "small_stmt.hpp"
}

%code {
// The root of the AST
Statement *ast;

// tmp Expr list for building list literals and tuples
// TODO: Got to be a safer way to do this. Consider nested lists!
std::vector<Expr *> tmp_expr_list;
std::vector<char*> tmp_str_list;
}

%define parse.error verbose
%locations

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

// Bison does precedence from lowest to highest
%left <op2> LAND LOR

%left <op2> EQ
%left <op2> LT LTE GT GTE

%left <op2> ADD SUB
%left <op2> MUL DIV MOD
%right NEG
%right <op1> LNOT

%nonassoc THEN
%nonassoc ELSE

%left ')'
%right '('

%token LAMBDA_OPEN "(\\"
%token LAMBDA_ARROW "->"
%token FUNC "func"
%token LINE_COMMENT "//"
%token IF "if" THEN "then" ELSE "else"

%token ENDL
%token RETURN

%type <expr> expr list tuple lambda app if
%type <stateval> program stmt seq func_body

%token END 0 "end of file"
%%

program:
    ENDLS seq   { $$ = $2; ast = $$; }
    | seq       { $$ = $1; ast = $$;}

seq:
   stmt ENDLS seq  { $$ = new Seq($1, $3); }
   | stmt ENDLS    { $$ = $1; }

stmt:
    ID '=' expr   { $$ = new Assign($1, $3); }
    | FUNC ID[name] id_list '=' '{' func_body[body] '}'
        { $$ = new Assign($name, new ELambda(tmp_str_list, $body)); tmp_str_list.clear(); }
    | FUNC ID[name] '=' '{' func_body[body] '}'
        { $$ = new Assign($name, new ELambda(tmp_str_list, $body)); tmp_str_list.clear(); }
    | RETURN expr { $$ = new Return($2); }

expr:
    INT     { $$ = new EInt($1); }
    | FLOAT  { $$ = new EFloat($1); }
    | ID     { $$ = new EId($1); }
    | STRING { $$ = new EString($1); }
    | CHAR   { $$ = new EChar($1); }
    | BOOL   { $$ = new EBool($1); }
    | '(' expr ')' { $$ = $2; }
    | list   { $$ = $1; }
    | tuple  { $$ = $1; }
    | lambda { $$ = $1; }
    | app    { $$ = $1; }
    | if     { $$ = $1; }
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

comma_sep_exprs:
    expr    { tmp_expr_list.push_back($1); }
    | comma_sep_exprs ',' expr { tmp_expr_list.push_back($3); }

list:
    '[' comma_sep_exprs ']' { $$ = new EList(tmp_expr_list); tmp_expr_list.clear(); }
    | '[' ']' { $$ = new EList(tmp_expr_list); tmp_expr_list.clear(); }

tuple_body:
          %empty
          | tuple_body ',' expr[e] { tmp_expr_list.push_back($e); }

tuple:
     '(' expr[e1]
        { tmp_expr_list.push_back($e1); }
     ',' expr[e2]
        { tmp_expr_list.push_back($e2); }
     tuple_body ')'
        { $$ = new ETuple(tmp_expr_list); tmp_expr_list.clear(); }
    | '(' ')' { $$ = new ETuple(); }

id_list:
       ID           { tmp_str_list.push_back($1); }
       | id_list ID { tmp_str_list.push_back($2); }

func_body:
         expr  { $$ = new Return($1); }
         | seq { $$ = $1; }

lambda:
      LAMBDA_OPEN id_list LAMBDA_ARROW func_body ')'
       { $$ = new ELambda(tmp_str_list, $4); tmp_str_list.clear(); }
      | LAMBDA_OPEN LAMBDA_ARROW func_body ')'
       { $$ = new ELambda(tmp_str_list, $3); tmp_str_list.clear(); }

app:
   expr[fun] '(' comma_sep_exprs ')'
      { $$ = new EApp($fun, tmp_expr_list); tmp_expr_list.clear(); }
   | expr '(' ')'
      { $$ = new EApp($1, tmp_expr_list); }

if:
  IF expr[cond] THEN expr[t_body] ELSE expr[f_body]
    { $$ = new EIf($cond, $t_body, $f_body); }

ENDLS:
     ENDL
     | ENDLS ENDL

%%

int main( int argc, char** argv) {
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        std::cout << "Failed to open " << argv[1] << std::endl;
        return 1;
    }

    while (!feof(yyin)) {
		yyparse();
    };
    std::cout << "Parsing completed." << std::endl;
    fclose(yyin);

    if (ast == NULL) {
        std::cout << "Parsing failed." << std::endl;
        return 2;
    } else {
        std::cout << "The program:\n" << ast->toString() << std::endl;;
    }
    return 0;
}

void yyerror(const char *msg) {
    std::cout << "Parse error at ";
    std::cout << yylloc.first_line << ":" << yylloc.first_column << " - ";
    std::cout << yylloc.last_line << ":" << yylloc.last_column << ": ";
    std::cout << msg << std::endl;
}
