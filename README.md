# COP3404 - Systems Software Fall 2020
This repo follows my coursework for the Systems Software course at UNF taken in the Fall 2020 semester.

## Project 1
This project involved building the functionality for pass 1 of an assembler for the SIC assembly language. This part of the assembler reads in a SIC assembly sourcecode file and performs validation checks to ensure it is syntactically correct, as well as building the symbol table for use in pass 2.

## Project 2
This project involved building on project 1 which resulted in the completion of pass 1 of an assembler for the SIC assembly language and adds the functionality for pass 2 of that assembly process. This pass through the source code involved reading through the source code a second time, hence the name 'pass 2', and checks to ensure that no symbol is defined on more than one line, and every symbol referenced in an operand has previously been defined. As well as performing those semantic checks, this pass also checks that the program begins at a valid memory location, as defined in the SIC assembly programmer's reference, and that it does not exceed the maximum memory bounds for a SIC machine. During pass 2, we also write the object code output line by line and, upon ensuring the entirety of the program is semantically correct, results in the entirety of the source code being translated into a single output object file with .sic.obj file extension.  

