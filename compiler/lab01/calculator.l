/* 
实现一个解析语法的程序（但是没有语法对应的动作）
*/
%{
/*
    enum yytokentype {
        NUMBER = 258,
        ADD = 260,
        SUB = 261,
        MUL = 262,
        DIV = 263,
        ABS = 264,
        EOL = 265
    };
    int yylval;
*/

/* token 在 yacc 文件中定义, 所以顺序是先定义 *.y 的文件 */
#include "y.tab.h"
%}

/* 定义词法的识别规则，这里对词法直接指定了语义，跳过了语法分析、语义分析 */
%%
"+" { return ADD; }
"-" { return SUB; }
"*" { return MUL; }
"/" { return DIV; }
"|" { return ABS; }
[0-9]+ { yylval = atoi(yytext); return NUMBER; }
\n { return EOL; }
[ \t] { /* 忽略空白字符 */ }
. { printf("Mystery character %c\n", *yytext); }
%%

/* 定义主函数 */
/*
main(int argc, char **agrv) {
  int tok;
  while(tok = yylex()) {
    printf("%d", tok);
    if (tok == NUMBER) {
      printf(" = %d\n", yylval);
    } else {
      printf("\n");
    }
  }
}
*/