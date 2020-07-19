/* 
测试往 yyin 中写入， 设置输入流
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

int main(int argc, char **argv) {
  
  // 没有接入参数
  if (argc < 2) {
    // 一次调用全部解析
    yylex();
  }

  for ( int i = 1; i < argc; i++) {
    FILE *f = fopen(argv[i], "r");
    // 文件打开失败
    if (!f) {
      perror(argv[i]);
      return 1;
    }

    // 重启分析程序
    yyrestart(f);
    yylex();
    fclose(f);
    printf("%d %d %d: %s\n", lines, words, chars, argv[i]);
    lines = 0; words = 0; chars = 0; 
  }
  return 0;
}

yywrap() {
  return 1;
}