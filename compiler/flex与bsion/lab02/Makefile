wc: wc.l
	flex wc.l
	# 其中，-ll表示链接lex的相关库文件，要想编译时不带-ll选项，就必须实现main函数和yywrap函数(return 1即可)。
	gcc   lex.yy.c -o wc
wc-test: wc
	./wc test.txt test.txt test.txt
clean:
	rm -f wc lex.yy.c y.tab.c  a.out