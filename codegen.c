#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"

#define CODE_SIZE 500

int cx = 0, numVars, lexIndex = 0, symTableIndex;
instruction* code;
token_type token;

instruction* generate_code(symbol *table, lexeme *list){

    code = malloc(500 * sizeof(instruction));
    token = list[lexIndex].value;
    
    //JMP
    emit(7, 0, 0, 1);
    block(code, table, list, token);

    //HALT
    emit(9, 0, 0, 3);

    printf("code has been generated\n");
    return code;
}

void block(instruction* code, symbol* table, lexeme* list, token_type token){
    numVars = 0;
    if(token == constsym){
        do{
            token = incToken(token, list, 4);
        }while(token == commasym);
        token = incToken(token, list, 1);
    }
    if(token == varsym){
        do{
            numVars++;
            token = incToken(token, list, 2);
        }while(token == commasym);
        token = incToken(token, list, 1);
    }
    //INC
    emit(6, 0, 0, 3+numVars);
    statement(table, list);
}

void statement(symbol* table, lexeme* list){
    if(token == identsym){
        symTableIndex = lexIndex;
        token = incToken(token, list, 2);
        expression(0);
        //STO
        int addr = table[symTableIndex].addr;
        emit(4, 0, 0, table[symTableIndex].addr);
    }
}
void emit(int op, int r, int l, int m){
    if (cx > CODE_SIZE){
        printf("ERROR: Code Is Too Large");
    }else{
        code[cx].op = op;
        code[cx].r = r;
        code[cx].l = l;
        code[cx].m = m;
        cx++;
    }
}

int incToken(token_type token, lexeme* list, int jump){
    lexIndex += jump;
    return list[lexIndex].type;
}