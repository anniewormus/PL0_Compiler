#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"

FILE * inF;        //input: Lexeme list
FILE * outF;

int lexIndex = 0, symTableIndex = 0, check;
token_type token;
symbol *table;

symbol* parser(lexeme* list){
    table = malloc(500 * sizeof(symbol));
    
    //inF = fopen("lexOUT.txt", "r");
   // outF = fopen("parserOUT.txt", "w");

    int i = 0;

    token = lexemeList[lexIndex].tokenVal;
    block(token);
    if(token != periodsym){
        printf("ERROR: Period Expected");
    }

    printf("symbols are parsed\n");
    return table;
}

void block(int token){
    if(token == constsym){
        constDeclataion();
    }else if(token == varsym){
        varDeclaration();
    }else{
        statement();
    }
}
//variable is declared as a constant
void constDeclaration(){
    int i;
    char * varName;
    do{
        lexIndex++;
        token = lexemeList[lexIndex].tokenVal;
        if(token != identsym){
            printf("ERROR: Const must be followed by identifier");
        }
    
        token = lexemeList[lexIndex].varname;

        //checks to see if the constant varible has already been declared
        check = symCheck(lexIndex, table, token);

        if(check == 0){     //check fails, identifier already exists
            printf("ERROR: Identifier Already Exists");
            return;
        }else if(check == 1){       //check passes
            strcpy(varName, token);
        }

        token = getNextToken(token);

        if(token != eqsym){
            printf("ERROR: Identifier Must Be Followed By =");
            return;
        }
    
        token = getNextToken(token);

        if(token != numbersym){
            printf("ERROR: = Must Be Followed By A Number");
            return;
        }

        //if everything checks out, add to symbol table
        addToSymTable(table, 1, varName, token, 0, 0, 0);

        token = getNextToken(token);

    } while(token == commasym);
    if(token != semicolonsym){
        printf("ERROR: ; Expected");
        return;
    }

    token = getNextToken(token);
}


void varDeclaration(){
    int numVars;
    if(token == varsym){
        numVars = 0;    //keep track of how many variables you have on the stack
        do{
            numVars++;
            token = getNextToken(token);

            if(token != identsym){
                printf("ERROR: Var Must Be Followed By An Identifier");
            }
            //check to see if ident is already in symbol table
            check = symCheck(lexIndex, table, token);
            if(check == 0){
                printf("ERROR: Identifier Already Exists");
            }else if(check == 1){
                addToSymTable(table, 2, lexemeList[lexIndex].varname, 0, 0, numVars + 2, 0);
            }

            token = getNextToken(token);
        }while(token == commasym);

        if(token != semicolonsym){
            printf("ERROR: ; Expected");
            return;
        }
        token = getNextToken(token);
    }
}

void statement(){
    switch(token){
        case identsym:
            //if ident is not in symbol table
            check = symCheck(symTableIndex, table, lexemeList[lexIndex].varname);
            if(check == 0){
                printf("ERROR: Undeclared Identifier");
            }
            token = getNextToken(token);

            if(token != becomessym){
                printf("ERROR: := expected");  
            }
            token = getNextToken(token);

            //EXPRESSION
            break;
        case beginsym:
            break;
        case ifsym:
            break;
        case whilesym:
            break;
        case readsym:
            break;
        case writesym:
            break;
    }
}

void condition(){

}
void expression(){

}
void term(){

}
void factor(){
    
}

//checks symbol table to see if there are conflicting values
int symCheck(int index, symbol * sym, char * token){
    int i;
    for(i = 0; i < index; i++){
        if(strcmp(sym[i].name, token) == 0){
            return 0;       //failed
        }else{
            return 1;       //passed
        }
    }
}

void addToSymTable(symbol* sym, int kind, char* name, int val, int lex, int m, int mark){
    table[symTableIndex].kind = kind;
    strcpy(table[symTableIndex].name, name);
    table[symTableIndex].val = val;
    table[symTableIndex].level = lex;
    table[symTableIndex].addr = m;
    table[symTableIndex].mark = mark;
    symTableIndex++;
}

int getNextToken(token_type token){
    lexIndex++;
    return lexemeList[lexIndex].tokenVal;
}

