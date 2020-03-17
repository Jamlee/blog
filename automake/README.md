文件的用途解析
---
- acloca.m4 中拷贝了其他工具中定义的宏。例如 `grep AM_AUX_DIR_EXPAND  -r  /usr/local/Cellar/automake`, 像是文章合并工具 把宏定义到一起。
- 使用 m4 可以更无脑的写 configure 脚本（configure 本意是用于读取 Makefile.in，config.h.in 来生成 Makefile 的）
- 重新定义 m4_define [m4_copy 换名字](https://github.com/autotools-mirror/autoconf/blob/d78a7dd95fed24e417dfb4d08124a9ce7c1f7d1e/lib/autoconf/general.m4#L157) 


参考:
https://www.jianshu.com/p/b3b0a090a01e

https://www.gnu.org/software/autoconf/manual/autoconf-2.69/html_node/Prerequisite-Macros.html#Prerequisite-Macros

中文列表: https://chungkwong.cc/m4.html

宏的教程: https://www.kancloud.cn/digest/gun-m4/99012

源码: https://github.com/autotools-mirror/autoconf

autotool 如何换名字: https://www.gnu.org/software/autoconf/manual/autoconf-2.68/html_node/Redefined-M4-Macros.html

m4_copy:https://github.com/autotools-mirror/m4/blob/d69fa5284851b2b7aac25ffac638b6b379994db5/bootstrap#L2419

m4 doc: https://www.gnu.org/savannah-checkouts/gnu/m4/manual/m4-1.4.18/m4.html
