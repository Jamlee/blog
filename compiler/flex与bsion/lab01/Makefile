wc:
	flex wc.l
	gcc -ll  lex.yy.c -o wc
wc-test: wc
	cat test.txt|  ./wc

calculator:
	yacc -d calculator.y
	flex calculator.l
	gcc -ll y.tab.c lex.yy.c -o calculator
calculator-test: calculator
	echo 1+2*3 |  ./calculator

clean:
	rm -f wc lex.yy.c y.tab.c calculator a.out