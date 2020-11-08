#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"

//method prototypes
symbol* parser(lexeme* list);
void block(symbol* table, int token);
void constDeclaration(symbol* table, int token);
void varDeclaration(symbol* table, int token);
void statement(symbol* table, int token);
void condition(int token);
void expression(int token);
void term(int token);
void factor(symbol* table, int token);
int symCheck(int index, symbol * sym, char * token);
void addToSymTable(symbol* sym, int kind, char* name, int val, int lex, int m, int mark);
int getNextToken(token_type token);
int relopCheck(int token, int sym);

int lexIndex = 0, symTableIndex = 0, check;
token_type token;
symbol *table;      //symbol table

symbol* parser(lexeme* list){
    table = malloc(500 * sizeof(symbol));
    
    int i = 0;

    token = lexemeList[lexIndex].tokenVal;
    block(table, token);

    //if program doesn't end in a period throw error
    if(token != periodsym){
        printf("ERROR: Period Expected");
    }

    printf("symbols are parsed\n");
    return table;
}

void block(symbol* table, int token){
    if(token == constsym){
        constDeclataion(table, token);
    }else if(token == varsym){
        varDeclaration(table, token);
    }else{
        statement(table, token);
    }
}
//variable is declared as a constant
void constDeclaration(symbol* table, int token){
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
        }else if(check == 1){       //check passes
            strcpy(varName, token);
        }

        token = getNextToken(token);

        if(token != eqsym){
            printf("ERROR: Identifier Must Be Followed By =");
        }
    
        token = getNextToken(token);

        if(token != numbersym){
            printf("ERROR: = Must Be Followed By A Number");
        }

        //if everything checks out, add to symbol table
        addToSymTable(table, 1, varName, token, 0, 0, 0);

        token = getNextToken(token);

    } while(token == commasym);
    if(token != semicolonsym){
        printf("ERROR: ; Expected");
    }

    token = getNextToken(token);
}

void varDeclaration(symbol* table, int token){
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

void statement(symbol* table, int token){
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

            expression(token);
            break;

        case beginsym:
            token = getNextToken(token);
            statement(table, token);
            while(token == semicolonsym){
                token = getNextToken(token);
                statement(table, token);
            }
            if(token != endsym){
                printf("ERROR: } Expected");
            }
            token = getNextToken(token);
            break;

        case ifsym:
            token = getNextToken(token);
            condition(token);
            if(token != thensym){
                printf("ERROR: then Expected");
            }
            token = getNextToken(token);
            statement(table, token);
            break;

        case whilesym:
            token = getNextToken(token);
            condition(token);
            if(token != dosym){
                printf("ERROR: do Expected");
            }
            token = getNextToken(token);
            statement(table, token);
            break;

        case readsym:
            token = getNextToken(token);
            if(token != identsym){
                printf("ERROR: Call Must Be Followed By An Identifier");
            }
            //check to see if identifier is on symbol table
            check = symCheck(symTableIndex, table, lexemeList[lexIndex].varname);
            if(check == 1){
                printf("ERROR: Undeclared Identifier");
            }
            //check if ident is a variable

            token = getNextToken(token);
            break;

        case writesym:
            token = getNextToken(token);
            if(token != identsym){
                printf("ERROR: Call Must Be Followed By An Identifier");
            }
            check = symCheck(symTableIndex, table, lexemeList[lexIndex].varname);
            if(check == 1){
                printf("ERROR: Undeclared Identifier");
            }
            token = getNextToken(token);
            break;
    }
}

void condition(int token){
    if(token == oddsym){
        token = getNextToken(token);
        expression(token);
    }else{
        expression(token);
        int rcheck = 0;
        rcheck = relopCheck(token, eqsym);
        rcheck = relopCheck(token, neqsym);
        rcheck = relopCheck(token, gtrsym);
        rcheck = relopCheck(token, geqsym);
        rcheck = relopCheck(token, lessym);
        rcheck = relopCheck(token, leqsym);
        if(rcheck != 1){
            printf("ERROR: Rel-Op Expected");
        }
        token = getNextToken(token);
        expression(token);
    }

}
void expression(int token){
    if(token == plussym || token == minussym){
        token = getNextToken(token);
    }
    term(token);
    while(token == plussym || token == minussym){
        token = getNextToken(token);
        term(token);
    }

}

void term(int token){
    factor(table, token);
    while(token == multsym || token == slashsym){
        token = getNextToken(token);
        factor(table, token);
    }
}

void factor(symbol* table, int token){
    if(token == identsym){
        check = symCheck(symTableIndex, table, lexemeList[lexIndex].varname);
            if(check == 1){
                printf("ERROR: Undeclared Identifier");
            }
        token = getNextToken(token);
    }else if(token == numbersym){
        token = getNextToken(token);
    }else if(token == lparentsym){
        token = getNextToken(token);
        expression(token);
        if(token != rparentsym){
            printf("ERROR: ) Expected");
        }
        token = getNextToken(token);
    }else{
        printf("ERROR: Call Of A Constant Or Variable Is Meaningless");
    }
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

int relopCheck(int token, int sym){
    if(token == sym){
        return 1;
    }
}

