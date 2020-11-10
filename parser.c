#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"

//method prototypes
symbol* parser(lexeme* list);
void pblock(symbol* table, lexeme* list);
void constDeclaration(symbol* table, lexeme* list);
void varDeclaration(symbol* table, lexeme* list);
void pstatement(symbol* table, lexeme* list);
void pcondition(lexeme* list);
void pexpression(lexeme* list);
void pterm(lexeme* list);
void pfactor(lexeme* list, symbol* table);
int symCheck(lexeme* list, symbol * sym, char * token);
void addToSymTable(symbol* sym, int kind, char* name, int val, int lex, int m, int mark);
int getNextToken(lexeme* list, int token);
int relopCheck(int token, int sym);

int plexIndex = 0, symTableIndex = 0, check, varFlag = 0;
token_type token;
symbol* table;      //symbol table
lexeme* list;

symbol* parse(lexeme* list){
    table = malloc(500 * sizeof(symbol));

    token = list[plexIndex].type;

    pblock(table, list);

    //printf("token: %d\n", list[plexIndex].type);

    //if program doesn't end in a period throw error
    if(token != periodsym){
        printf("ERROR: Period Expected");
        exit(0);
    }
   // int i;
    printf("symbols are parsed\n");
    // for(i = 0; i < symTableIndex; i++){
    //     printf("TABLE: %d ", table[i].addr);
    // }
    return table;
}

void pblock(symbol* table, lexeme* list){
    //printf("in pblock - %d\n", token);
        constDeclaration(table, list);
        varDeclaration(table, list);
        pstatement(table, list);
}
//variable is declared as a constant
void constDeclaration(symbol* table, lexeme* list){
    if(token == constsym){
        // printf("in constdec - %d\n", token);
    char * varName;
    do{
        token = getNextToken(list, token);
        if(token != identsym){
            printf("ERROR: Const must be followed by identifier");
            exit(0);
        }
    
        //checks to see if the constant varible has already been declared
        check = symCheck(list, table, list[plexIndex].name);

        if(check == 0){     //check fails, identifier already exists
            printf("ERROR: Identifier Already Exists");
            exit(0);
        }else if(check == 1){       //check passes
            strcpy(varName, list[plexIndex].name);
        }

        token = getNextToken(list, token);

        if(token != eqsym){
            printf("ERROR: Identifier Must Be Followed By =");
            exit(0);
        }
    
        token = getNextToken(list, token);

        if(token != numbersym){
            printf("ERROR: = Must Be Followed By A Number");
            exit(0);
        }

        //if everything checks out, add to symbol table
        addToSymTable(table, 1, varName, token, 0, 0, 0);

        token = getNextToken(list, token);

    } while(token == commasym);
    if(token != semicolonsym){
        printf("ERROR: ; Expected");
        exit(0);
    }

    token = getNextToken(list, token);
    }
   
    // pblock(table, list);
}

void varDeclaration(symbol* table, lexeme* list){
    if(token == varsym){
         //printf("in vardec - %d\n", token);
    int numVars;
        numVars = 0;    //keep track of how many variables you have on the stack
        do{
            numVars++;
            token = getNextToken(list, token);

            if(token != identsym){
                printf("ERROR: Var Must Be Followed By An Identifier");
                exit(0);
            }
            //check to see if ident is already in symbol table
            check = symCheck(list, table, list[plexIndex].name);
            if(check == 0){
                printf("ERROR: Identifier Already Exists");
                exit(0);
            }else if(check == 1){
                addToSymTable(table, 2, list[plexIndex].name, 0, 0, numVars + 2, 0);
            }

            token = getNextToken(list, token);
        }while(token == commasym);

        if(token != semicolonsym){
            printf("ERROR: ; Expected");
            exit(0);
        }
        token = getNextToken(list, token);
        // pblock(table, list);
    }
   
}

void pstatement(symbol* table, lexeme* list){
    //printf("in pstate - %d\n", token);
    switch(token){
        case identsym:
            //if ident is not in symbol table
            check = symCheck(list, table, list[plexIndex].name);
            if(check == 1){
                printf("ERROR: Undeclared Identifier");
                exit(0);
            }
            token = getNextToken(list, token);

            if(token != becomessym){
                printf("ERROR: := expected");
                exit(0);  
            }
            token = getNextToken(list, token);

            pexpression(list);
            break;

        case beginsym:
            token = getNextToken(list, token);
            pstatement(table, list);
            while(token == semicolonsym){
                token = getNextToken(list, token);
                pstatement(table, list);
            }
            if(token != endsym){
                printf("ERROR: } Expected");
                exit(0);
            }
            token = getNextToken(list, token);
            break;

        case ifsym:
            token = getNextToken(list, token);
            pcondition(list);
            if(token != thensym){
                printf("ERROR: then Expected");
                exit(0);
            }
            token = getNextToken(list, token);
            pstatement(table, list);
            break;

        case whilesym:
            token = getNextToken(list, token);
            pcondition(list);
            if(token != dosym){
                printf("ERROR: do Expected");
                exit(0);
            }
            token = getNextToken(list, token);
            pstatement(table, list);
            break;

        case readsym:
            token = getNextToken(list, token);
            if(token != identsym){
                printf("ERROR: Call Must Be Followed By An Identifier");
                exit(0);
            }
            //check to see if identifier is on symbol table
            check = symCheck(list, table, list[plexIndex].name);
            if(check == 1){
                printf("ERROR: Undeclared Identifier");
                exit(0);
            }
            //check if ident is a variable
            token = getNextToken(list, token);
            break;

        case writesym:
            token = getNextToken(list, token);
            if(token != identsym){
                printf("ERROR: Call Must Be Followed By An Identifier");
                exit(0);
            }
            check = symCheck(list, table, list[plexIndex].name);
            if(check == 1){
                printf("ERROR: Undeclared Identifier");
                exit(0);
            }
            token = getNextToken(list, token);
            break;
    }
    // pblock(table, list);
}

void pcondition(lexeme* list){
    //printf("in pcond\n");
    if(token == oddsym){
        token = getNextToken(list, token);
        pexpression(list);
    }else{
        pexpression(list);
        int rcheck = 0;
        rcheck = relopCheck(token, eqsym);
        rcheck = relopCheck(token, neqsym);
        rcheck = relopCheck(token, gtrsym);
        rcheck = relopCheck(token, geqsym);
        rcheck = relopCheck(token, lessym);
        rcheck = relopCheck(token, leqsym);
        if(rcheck != 1){
            printf("ERROR: Rel-Op Expected");
            exit(0);
        }
        token = getNextToken(list, token);
        pexpression(list);
    }

}
void pexpression(lexeme* list){
    //printf("in pex - %d\n", token);
    if(token == plussym || token == minussym){
        token = getNextToken(list, token);
    }
    pterm(list);
    //printf("AFTER PTERM IN PEX = %d\n", token);
    while(token == plussym || token == minussym){
        //printf("IN PEX TOKEN = %d\n", token);
        token = getNextToken(list, token);
        pterm(list);
    }

}

void pterm(lexeme* list){
    //printf("in pterm - %d\n", token);
    pfactor(list, table);
    //printf("AFTER PFACT IN PTERM = %d\n", token);
    while(token == multsym || token == slashsym){
        token = getNextToken(list, token);
        pfactor(list, table);
    }
}

void pfactor(lexeme* list, symbol* table){
    //printf("in pfact - %d\n", token);
    if(token == identsym){
        check = symCheck(list, table, list[plexIndex].name);
            if(check == 0){
                printf("ERROR: Undeclared Identifier");
                exit(0);
            }
        token = getNextToken(list, token);
        //printf("DONE W PFACT = %d\n", token);
    }else if(token == numbersym){
        token = getNextToken(list, token);
    }else if(token == lparentsym){
        token = getNextToken(list, token);
        pexpression(list);
        if(token != rparentsym){
            printf("ERROR: ) Expected");
            exit(0);
        }
        token = getNextToken(list, token);
    }else{
        printf("ERROR: Call Of A Constant Or Variable Is Meaningless");
        exit(0);
    }
}

//checks symbol table to see if there are conflicting values
int symCheck(lexeme* list, symbol * sym, char * token){
    //printf("in symcheck - %d", symTableIndex);
    int i;
    for(i = 0; i < symTableIndex; i++){
        //printf("%s %s\n", sym[i].name, token);
        if(strcmp(sym[i].name, token) == 0){
            return 0;       //already in table
        }else{
            return 1;       //not in table
        }
    }
    return 1;
}

void addToSymTable(symbol* sym, int kind, char* name, int val, int lex, int m, int mark){
   // printf("in add\n");
    table[symTableIndex].kind = kind;
    strcpy(table[symTableIndex].name, name);
    table[symTableIndex].val = val;
    table[symTableIndex].level = lex;
    table[symTableIndex].addr = m;
    table[symTableIndex].mark = mark;
    symTableIndex++;
}

int getNextToken(lexeme* list, int token){
    //printf("in nexttok\n");
    plexIndex++;
    return list[plexIndex].type;
    
}

int relopCheck(int token, int sym){
    //printf("in relop\n");
    if(token == sym){
        return 1;
    }else{
        return 0;
    }
}

