#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"

//method prototypes
symbol* parser(lexeme* list);
void pblock(symbol* table, lexeme* list, int token);
void constDeclaration(symbol* table, int token);
void varDeclaration(symbol* table, int token);
void pstatement(symbol* table, int token);
void pcondition(int token);
void pexpression(int token);
void pterm(int token);
void pfactor(symbol* table, int token);
int symCheck(int index, symbol * sym, char * token);
void addToSymTable(symbol* sym, int kind, char* name, int val, int lex, int m, int mark);
int getNextToken(token_type token);
int relopCheck(int token, int sym);

int plexIndex = 0, symTableIndex = 0, check;
token_type token;
symbol* table;      //symbol table
lexeme* list;

symbol* parse(lexeme* list){
    table = malloc(500 * sizeof(symbol));

    token = list[plexIndex].type;
    pblock(table, list, token);

    //if program doesn't end in a period throw error
    if(token != periodsym){
        printf("ERROR: Period Expected");
    }

    printf("symbols are parsed\n");
    return table;
}

void pblock(symbol* table, lexeme* list, int token){
    if(token == constsym){
        constDeclaration(table, token);
    }else if(token == varsym){
        varDeclaration(table, token);
    }else{
        pstatement(table, token);
    }
}
//variable is declared as a constant
void constDeclaration(symbol* table, int token){
    char * varName;
    do{
        plexIndex++;
        token = list[plexIndex].type;
        if(token != identsym){
            printf("ERROR: Const must be followed by identifier");
        }
    
        //checks to see if the constant varible has already been declared
        check = symCheck(plexIndex, table, list[plexIndex].name);

        if(check == 0){     //check fails, identifier already exists
            printf("ERROR: Identifier Already Exists");
        }else if(check == 1){       //check passes
            strcpy(varName, list[plexIndex].name);
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
        addToSymTable(table, 1, list[plexIndex].name, token, 0, 0, 0);

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
            check = symCheck(plexIndex, table, list[plexIndex].name);
            if(check == 0){
                printf("ERROR: Identifier Already Exists");
            }else if(check == 1){
                addToSymTable(table, 2, list[plexIndex].name, 0, 0, numVars + 2, 0);
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

void pstatement(symbol* table, int token){
    switch(token){
        case identsym:
            //if ident is not in symbol table
            check = symCheck(symTableIndex, table, list[plexIndex].name);
            if(check == 0){
                printf("ERROR: Undeclared Identifier");
            }
            token = getNextToken(token);

            if(token != becomessym){
                printf("ERROR: := expected");  
            }
            token = getNextToken(token);

            pexpression(token);
            break;

        case beginsym:
            token = getNextToken(token);
            pstatement(table, token);
            while(token == semicolonsym){
                token = getNextToken(token);
                pstatement(table, token);
            }
            if(token != endsym){
                printf("ERROR: } Expected");
            }
            token = getNextToken(token);
            break;

        case ifsym:
            token = getNextToken(token);
            pcondition(token);
            if(token != thensym){
                printf("ERROR: then Expected");
            }
            token = getNextToken(token);
            pstatement(table, token);
            break;

        case whilesym:
            token = getNextToken(token);
            pcondition(token);
            if(token != dosym){
                printf("ERROR: do Expected");
            }
            token = getNextToken(token);
            pstatement(table, token);
            break;

        case readsym:
            token = getNextToken(token);
            if(token != identsym){
                printf("ERROR: Call Must Be Followed By An Identifier");
            }
            //check to see if identifier is on symbol table
            check = symCheck(symTableIndex, table, list[plexIndex].name);
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
            check = symCheck(symTableIndex, table, list[plexIndex].name);
            if(check == 1){
                printf("ERROR: Undeclared Identifier");
            }
            token = getNextToken(token);
            break;
    }
}

void pcondition(int token){
    if(token == oddsym){
        token = getNextToken(token);
        pexpression(token);
    }else{
        pexpression(token);
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
        pexpression(token);
    }

}
void pexpression(int token){
    if(token == plussym || token == minussym){
        token = getNextToken(token);
    }
    pterm(token);
    while(token == plussym || token == minussym){
        token = getNextToken(token);
        pterm(token);
    }

}

void pterm(int token){
    pfactor(table, token);
    while(token == multsym || token == slashsym){
        token = getNextToken(token);
        pfactor(table, token);
    }
}

void pfactor(symbol* table, int token){
    if(token == identsym){
        check = symCheck(symTableIndex, table, list[plexIndex].name);
            if(check == 1){
                printf("ERROR: Undeclared Identifier");
            }
        token = getNextToken(token);
    }else if(token == numbersym){
        token = getNextToken(token);
    }else if(token == lparentsym){
        token = getNextToken(token);
        pexpression(token);
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
    return 1;
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
    plexIndex++;
    return list[plexIndex].type;
}

int relopCheck(int token, int sym){
    if(token == sym){
        return 1;
    }else{
        return 0;
    }
}

