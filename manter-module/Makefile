make: foo
foo:
	g++ -c -std=c++11 -Wall -Werror -fpic foo.c
	g++ -shared -o libfoo.so foo.o
	sudo cp libfoo.so /usr/lib
main:
	g++ -I. -L. -O2 -Wall -o test main.c libfoo.so
