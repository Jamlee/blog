/* 
实现一个 wc 的程序。但是在re2c中这个语法有变化了
在声明部分 %{ 和 %} 会原样生成到 C 文件里. 这里 %{ %} 注意 % 有点类似于转义符号了
*/
%{
  int chars = 0;
  int words = 0;
  int lines = 0; 
%}

/* 定义词法的识别规则，这里对词法直接指定了语义，跳过了语法分析、语义分析 */
%%
[a-zA-Z]+ { words++; chars += strlen(yytext);}
\n        { lines++; chars++;}
.         { chars++; }
%%

main(int argc, char **agrv) {
  /* 执行分析 */
  yylex();
  printf("%d %d %d\n", lines, words, chars);
}
