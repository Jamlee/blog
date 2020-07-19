%{
#include <stdio.h>
%}

/* 先定义 token和语法规则，再完成词法分析 */
%token NUMBER
%token ADD SUB MUL DIV ABS
%token EOL

/* 规则, 每个语法对应的有相应的语义动作 */
%%
calclist: 
    /* 空规则 */
    | calclist exp EOL { printf(" = %d\n", $2); }
    ;

exp: 
    factor { $$ = $1; }
    | exp ADD factor { $$ = $1 + $3; }
    | exp SUB factor { $$ = $1 - $3; }
    ;

factor: 
    term { $$ = $1; }
    | exp MUL factor { $$ = $1 * $3; }
    | exp DIV factor { $$ = $1 / $3; }
    ;

term: 
    NUMBER { $$ = $1; }
    | ABS term  { $$ = $2 > 0 ? $2 : -$2; }
    ;
%%

int main(int argc, char **argv) {
    yyparse();
    return 0;
}

yyerror(char *s) {
    printf(stderr, "error: %s \n", s);
}