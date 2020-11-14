assemble: prj2defs.c
	gcc -Wall -DASSEMBLE -c prj2defs.c
	gcc -Wall -o assembler prj2main.c prj2defs.o
mac: assemble.c
	clang -Wall -DASSEMBLE -c prj2defs.c
	clang -Wall -o assembler prj2main.c prj2defs.o
clean:
	rm -f assembler
