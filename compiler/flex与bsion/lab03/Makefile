calculator: fb3-1.l
	yacc -d -o fb3-1.tab.c fb3-1.y 
	flex -o fb3-1.yy.c fb3-1.l
	gcc -o calculator fb3-1.yy.c fb3-1.tab.c fb3-1funcs.c
calculator-test: calculator
	./calculator
clean:
	rm -f calculator lex.yy.c y.tab.c  a.out fb3-1.yy.c fb3-1.tab.c 