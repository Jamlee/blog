文件的用途解析
---
- acloca.m4 中拷贝了其他工具中定义的宏。例如 `grep AM_AUX_DIR_EXPAND  -r  /usr/local/Cellar/automake`, 像是文章合并工具 把宏定义到一起。
- 使用 m4 可以更无脑的写 configure 脚本（configure 本意是用于读取 Makefile.in，config.h.in 来生成 Makefile 的）


参考:
https://www.jianshu.com/p/b3b0a090a01e
https://www.gnu.org/software/autoconf/manual/autoconf-2.69/html_node/Prerequisite-Macros.html#Prerequisite-Macros
https://chungkwong.cc/m4.html