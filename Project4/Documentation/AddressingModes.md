# Addressing Modes Explained
## Indirect Addressing
- Indirect addressing is an addressing mode that can be thought of as being conceptually similar to pointers in C
- A pointer in C is what? It is a variable whose value is the address of another variable
- How would you get the value of the variable stored in the address specified by a pointer? You have to *dereference* the pointer
	- This is done by using the dereference operator 
	- Example: 
	
		``int x = 10;``  
		``int* intP = &x;``
		``printf("x value: %d", x);`` -> Result: 10    
		``printf("x's address: %p", intP);`` -> Result: 0x7ffffe348a8c    
		``printf("Using x's pointer to get it's value: %d", *intP);`` -> Result: 10
	
- Indirect addressing in the same way. To specify you want to use indirect addressing in SIC/XE, the programmer prepends the *@* character to the symbol containing the address of the symbol whose value they want to access
	- Example: 

		``J @RETADR``    
		This instruction says to jump to the address that is stored as the value of the symbol whose address is stored in the RETADR symbol 