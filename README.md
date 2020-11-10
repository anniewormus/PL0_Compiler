# PL0_Compiler
A combination of a Virtual Machine and Lexical Analyzer to create a functioning compiler

TO COMPILE:
Run the following command in your terminal:
    gcc driver.c lex.c parser.c codegen.c vm.c -o run.exe
followed by:
    ./run.exe
The following command line prompt can be used to see different outputs of the compiler:
    -l: prints the list of lexemes/token (scanner output) to the screen
    -a: prints the generated assembly code (parser/codegen output) to the screen
    -v: prints the virtual machine execution trace (virtual machine output) to the screen
For example:
    ./run.exe -l -a -v    //prints all types of output to the console
    ./run.exe -v          //prints only the virtual machine execution trace to the console
    ./run.exe             //prints nothing to the console
    
    
