#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"

FILE * inF;        //input: Lexeme list
FILE * outF;

int lexIndex = 0, symTableIndex = 0;
token_type token;

void parser(void){
    //inF = fopen("lexOUT.txt", "r");
   // outF = fopen("parserOUT.txt", "w");

    int i = 0;

    token = lexemeList[lexIndex].tokenVal;
    block(token);
    if(token != periodsym){
        printf("ERROR: Period Expected");
    }

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
void constDeclaration(){
    do{
        token = lexemeList[lexIndex].tokenVal;
        if(token != identsym){
            printf("ERROR: Const must be followed by identifier");
        }
        lexIndex++;
        token = lexemeList[lexIndex].varname;

        toSymTable(token, symTableIndex);

    } while(token == commasym);
}
void varDeclaration(){

}
void statement(){

}

void toSymTable(token_type token, int index){
    int i;

    for(i = 0; i < index; i++){
        if()
    }
}
// int getToken(FILE * inF){
//     int c;
//     fscanf(inF, "%d", &c);
//     return c;
// }
