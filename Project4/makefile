all: prj4defs.c
	gcc -Wall -DASSEMBLE -c prj4defs.c
	gcc -Wall -o project4 prj4v2.c prj4defs.o
debug: prj4defs.c
	gcc -Wall -DASSEMBLE  -c prj4defs.c
	gcc -g -Wall -o project4debug -DDEBUG prj4v2.c prj4defs.o
mac: prj4defs.c
	clang -Wall -DASSEMBLE -c prj4defs.c
	clang -Wall -o project4 prj4v2.c prj4defs.o
clean:
	rm -f project4 project4debug
