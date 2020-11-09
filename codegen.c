#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"

#define CODE_SIZE 500

int cx = 0, numVars, lexIndex = 0, symTabIndx, curCodeIndx;
instruction* code;
token_type token;

//method prototypes
instruction* generate_code(symbol *table, lexeme *list);
void block(instruction* code, symbol* table, lexeme* list, token_type token);
void statement(symbol* table, lexeme* list);
void condition(lexeme* list, symbol* table);
void expression(lexeme* list, symbol* table, int regdest);
void term(lexeme* list, symbol* table, int reg);
void factor(lexeme* list, symbol* table, int reg);
void emit(int op, int r, int l, int m);
int incToken(token_type token, lexeme* list, int jump);

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
    switch(token){
        case identsym:
            symTabIndx = lexIndex;
            token = incToken(token, list, 2);
            expression(list, table, 0);
            //STO
            emit(4, 0, 0, table[symTabIndx].addr);
            break;

        case beginsym:
            token = incToken(token, list, 1);
            statement(table, list);
            while(token == semicolonsym){
                token = incToken(token, list, 1);
                statement(table, list);
            }
            token = incToken(token, list, 1);
            break;
        case ifsym:
            token = incToken(token, list, 1);
            condition(list, table);
            curCodeIndx = cx;
            //JPC
            emit(8, 0, 0, 0);
            token = incToken(token, list, 1);
            statement(table, list);
            code[curCodeIndx].m = cx;
            break;
        case whilesym:
            token = incToken(token, list, 1);
            curCodeIndx = cx;
            condition(list, table);
            token = incToken(token, list, 1);
            int jumpIndx = cx;
            //JPC
            emit(8, 0, 0, 0);
            statement(table, list);
            //JMP
            emit(7, 0, 0, curCodeIndx);
            code[jumpIndx].m = cx;
            break;
        case readsym:
            token = incToken(token, list, 1);
            symTabIndx = lexIndex;
            token = incToken(token, list, 1);
            //READ
            emit(9, 0, 0, 2);
            //STO
            emit(4, 0, 0, table[symTabIndx].addr);
            break;
        case writesym:
            token = incToken(token, list, 1);
            symTabIndx = lexIndex;
            if(token == varsym){
                //LOD
                emit(3, 0, 0, table[symTabIndx].addr);
                //WRITE
                emit(9, 0, 0, 1);
            }
            if(token == constsym){
                //LIT
                emit(1, 0, 0, table[symTabIndx].val);
                //WRITE
                emit(9, 0, 0, 1);
            }
            token = incToken(token, list, 1);
            break;
    }
}
void condition(lexeme* list, symbol* table){
    if(token == oddsym){
        token = incToken(token, list, 1);
        expression(list, table, 0);
        //ODD
        emit(15, 0, 0, 0);
    }else{
        expression(list, table, 0);
        switch(token){
            case eqsym:
                token = incToken(token, list, 1);
                expression(list, table, 1);
                emit(17, 0, 0, 1);  //EQL
                break;
            case neqsym:
                token = incToken(token, list, 1);
                expression(list, table, 1);
                emit(18, 0, 0, 1);  //NEQ
                break;
            case lessym:
                token = incToken(token, list, 1);
                expression(list, table, 1);
                emit(19, 0, 0, 1);  //LSS
                break;
            case leqsym:
                token = incToken(token, list, 1);
                expression(list, table, 1);
                emit(20, 0, 0, 1);  //LEQ
                break;
            case gtrsym:
                token = incToken(token, list, 1);
                expression(list, table, 1);
                emit(21, 0, 0, 1);  //GTR
                break;
            case geqsym:
                token = incToken(token, list, 1);
                expression(list, table, 1);
                emit(22, 0, 0, 1);  //GEQ
                break;
        }
    }
}
void expression(lexeme* list, symbol* table, int regdest){
    if(token == plussym){
        token = incToken(token, list, 1);
    }
    if(token == minussym){
        token = incToken(token, list, 1);
        term(list, table, regdest);
        emit(10, regdest, 0, 0);    //NEG
        while(token == plussym || token == minussym){
            if(token == plussym){
                token = incToken(token, list, 1);
                term(list, table, regdest + 1);
                emit(11, regdest, regdest, regdest+1);  //ADD
            }   
            if(token == minussym){
                token = incToken(token, list, 1);
                term(list, table, regdest + 1);
                emit(12, regdest, regdest, regdest + 1);    //SUB
            }
        }
    }
     term(list, table, regdest);
        while(token == plussym || token == minussym){
            if(token == plussym){
                token = incToken(token, list, 1);
                term(list, table, regdest + 1);
                emit(11, regdest, regdest, regdest+1);  //ADD
            }   
            if(token == minussym){
                token = incToken(token, list, 1);
                term(list, table, regdest + 1);
                emit(12, regdest, regdest, regdest + 1);    //SUB
            }
        }

}
void term(lexeme* list, symbol* table, int reg){
    factor(list, table, reg);
    while(token == multsym || token == slashsym){
        if(token == multsym){
            token = incToken(token, list, 1);
            factor(list, table, reg + 1);
            emit(13, reg, reg, reg + 1);    //MUL
        }
        if(token == slashsym){
            token = incToken(token, list, 1);
            factor(list, table, reg + 1);
            emit(14, reg, reg, reg + 1);    //DIV
        }
    }
}
void factor(lexeme* list, symbol* table, int reg){
    if(token == identsym){
        symTabIndx = lexIndex; 
        if(token == constsym){
                emit(1, reg, 0, table[symTabIndx].val); //LIT
            }
            if(token == varsym){
                emit(3, reg, 0, table[symTabIndx].addr);   //LOD
            }
        token = incToken(token, list, 1);
    }else if(token == numbersym){
        emit(1, reg, 0, list[lexIndex].value);
        token = incToken(token, list, 1);
    }else{
        token = incToken(token, list, 1);
        expression(list, table, reg);
        token = incToken(token, list, 1);
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