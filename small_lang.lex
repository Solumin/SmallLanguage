%option noyywrap
%{
#include <stdio.h>

#include "small_lang.tab.h"

#define YY_DECL extern "C" int yylex()

int mylineno = 1;
%}

/* Why define alpha ourselves instead of using [:alpha:]?
 * So we can update this to UTF-8 easily! [:alpha:] maps to C's isAlpha,
 * which is not necessarily UTF-8 aware. */
alpha   [A-Za-z]
digit   [0-9]
id      ({alpha}|_)({alpha}|_|{digit})*

bool    (true|false)

intdec  [+-]?{digit}+
inthex  0x[0-9A-Fa-f]+
int     ({intdec}|{inthex})

/* Somewhat simplified def. of floats */
floata  {intdec}\.{digit}+
floate  {floata}(E|e){digit}+
float   ({floata}|{floate})

charc   [^']
charx   \\x{digit}{2}
chare   \\[n']
char    \'({charc}|{charx}|{chare})\'

string  \"[^\n"]*\"
/*mulstr  \"{3}.+\"{3}*/

ws      [ \t]+

%%

{ws}

\n      mylineno++;

{int}   {
    printf("%d: integer `%s`\n", mylineno, yytext);
    if (strncmp(yytext, "0x", 2) == 0) {
        yylval.ival = (int)strtol(yytext, NULL, 16);
    } else {
        yylval.ival = atoi(yytext);
    }
    return INT;
}

{float} {
    printf("%d: float `%s`\n", mylineno, yytext);
    yylval.fval = atof(yytext);
    return FLOAT;
}

{bool}  {
    printf("%d: bool `%s`\n", mylineno, yytext);
    if (yyleng == 4 && strncmp(yytext, "true", 4) == 0)
        yylval.boollit = true;
    else
        yylval.boollit = false;
    return BOOL;
}

{id}    {
    yylval.id = strndup(yytext, yyleng);
    printf("%d: ident `%s`\n", mylineno, yylval.id);
    return ID;
}

{string} {
    yylval.strlit = strndup(yytext, yyleng);
    printf("%d: string `%s`\n", mylineno, yylval.strlit);
    return STRING;
}

{char}  {
    printf("%d: char `%s`\n", mylineno, yytext);
    if (yyleng == 3) {
        yylval.charlit = yytext[1];
    } else if (strncmp(yytext, "'\\x", 3) == 0) {
        printf("\t hex char \n");
        yylval.charlit = (char)strtol(yytext+3,NULL,16);
    } else {
        yylval.charlit = yytext[2];
    }
    return CHAR;
}

.       printf("%d: ERROR `%s`\n", mylineno, yytext);

%%
