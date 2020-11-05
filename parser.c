#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"

FILE * inF;        //input: tokens - output of scanner
FILE * outF;

void parser(void){
    inF = fopen("lexOUT.txt", "r");
    outF = fopen("parserOUT.txt", "w");

    char c = fgetc(inF);

    while(c != EOF){
        if(c != '.'){
            
        }
    }

    //populate tokens array
   

}