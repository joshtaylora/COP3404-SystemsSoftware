assemble: prj4defs.c
	gcc -Wall -DASSEMBLE -c prj4defs.c
	gcc -Wall -o project4 project4.c prj4defs.o
debug: prj4defs.c
	gcc -Wall -DASSEMBLE -c prj4defs.c
	gcc -Wall -o project4 -DDEBUG prj4.c prj4defs.o
mac: prj3defs.c
	clang -Wall -DASSEMBLE -c prj4defs.c
	clang -Wall -o project4 project4.c prj4defs.o
clean:
	rm -f project4
