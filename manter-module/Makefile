make: foo
utwente:
	g++ -c -std=c++11 -Wall -Werror -fpic utwente.c
	g++ -shared -o libutwente.so utwente.o
	sudo cp libutwente.so /usr/lib
foo: utwente
	g++ -c -std=c++11 -Wall -Werror -fpic foo.c
	g++ -shared -o libfoo.so foo.o libutwente.so
	sudo cp libfoo.so /usr/lib
main:
	g++ -I. -L. -O2 -Wall -o test main.c libfoo.so
