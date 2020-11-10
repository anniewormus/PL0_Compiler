// COP3402
// 10.22.2020 - Andrea Wormus
// This code demonstrates a functioning compiler for the PL/0 language

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lex.h"

//array for reserved words names
char *reservedWords[] = {"const", "var", "procedure", "call", "begin", "end", "if", "then", "else", "while", "do", "read", "write", "odd"};

//array for special symbols
char specialSyms[] = {'+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>', ';', ':'};

FILE *output;

lexeme* lex_analyze(char *inputfile)
{
    lexeme *list = malloc(500 * sizeof(lexeme));

    int peek = 0; //used in the event of multiple characater tokens

    tokenStruct lexemeList[3000]; //array of token structs
    int lexcount = 0;   //keeps track of array location
    

    output = fopen("lexout.txt", "w");

    int i = 0;
    //char c = inputfile[i]; //stores each character in file

    //print the source code from the file
    //printf("SOURCE CODE:\n%s", inputfile);
    

    //fclose(input);

    //reset file
    //input = fopen(inputfile, "r");
    int indx = 0;
    char c = inputfile[indx];

    while (c != '\0')
    {
        //ignore tabs, white spaces, and newlines
        if (iscntrl(c) || isspace(c))
        {
            c = inputfile[++indx];
            continue;
        }
        else if (isalpha(c))
        {
            int length = 0;
            char word[12] = {0}; //12 chars is error identifier length

            word[0] = c;
            length++;
            peek = 1;
            c = inputfile[++indx];
            while (isalpha(c) || isdigit(c))
            {
                //errors out if identifier is longer than 11 characters
                if (length > 11)
                {
                    // printf("\nERROR: Name too long.");
                    lexemeList[lexcount].errorType = 3;      
                    while (isalpha(c) || isdigit(c))
                    {
                        c = inputfile[++indx];
                    }
                    peek = 0;
                    break;
                }
                word[length] = c;
                c = inputfile[++indx];
                length++;
                
            }
            //check to see if word is a reserve word
            int i = 0;
            int reswordcheck = 100;
            for (i; i < 14; i++)
            {
                if (strcmp(word, reservedWords[i]) == 0)
                {
                    reswordcheck = i;
                    strcpy(lexemeList[lexcount].varname, word);
                }
            }
            switch (reswordcheck)
            {
            case 0:
                lexemeList[lexcount].tokenVal = constsym;
                lexemeList[lexcount].kind = 1;
                break;
            case 1:
                lexemeList[lexcount].tokenVal = varsym;
                lexemeList[lexcount].kind = 2;
                break;
            case 2:
                lexemeList[lexcount].tokenVal = procsym;
                lexemeList[lexcount].kind = 3;
                break;
            case 3:
                lexemeList[lexcount].tokenVal = callsym;
                break;
            case 4:
                lexemeList[lexcount].tokenVal = beginsym;
                break;
            case 5:
                lexemeList[lexcount].tokenVal = endsym;
                break;
            case 6:
                lexemeList[lexcount].tokenVal = ifsym;
                break;
            case 7:
                lexemeList[lexcount].tokenVal = thensym;
                break;
            case 8:
                lexemeList[lexcount].tokenVal = elsesym;
                break;
            case 9:
                lexemeList[lexcount].tokenVal = whilesym;
                break;
            case 10:
                lexemeList[lexcount].tokenVal = dosym;
                break;
            case 11:
                lexemeList[lexcount].tokenVal = readsym;
                break;
            case 12:
                lexemeList[lexcount].tokenVal = writesym;
                break;
            case 13:
                lexemeList[lexcount].tokenVal = oddsym;
                break;
            default:
                lexemeList[lexcount].tokenVal = identsym;
                strcpy(lexemeList[lexcount].varname, word);
                break;
            }
            lexcount++;
        }
        else if (isdigit(c))
        {
            int digit = c - '0';
            int digLength = 1;
            c = inputfile[++indx];
            peek = 1;

            while (isdigit(c))
            {

                if (digLength > 5)
                {
                    // printf("\nERROR: Number too long");
                    lexemeList[lexcount].errorType = 2;
                    while (isdigit(c))
                    {
                        c = inputfile[++indx];
                    }
                    break;
                }
                digit = 10 * digit + (c - '0');
                digLength++;
                c = inputfile[++indx];
            }

            //throws error if the variable starts with a number
            if (isalpha(c))
            {
               // printf("ERROR: Variable does not start with letter");
                lexemeList[lexcount].errorType = 1;
                c = inputfile[++indx];
                while (isalpha(c) || isdigit(c))
                {
                    c = inputfile[++indx];
                }
            }
            lexemeList[lexcount].numVal = digit;
            lexemeList[lexcount].tokenVal = numbersym;
            lexcount++;
        }
        else
        {
            //if the character is a special symbol
            peek = 0;
            int i;
            int sym;
            for (i = 0; i < 13; i++)
            {
                if (c == specialSyms[i])
                {
                    sym = i;
                }
            }
            switch (sym)
            {
            //+
            case 0:
                lexemeList[lexcount].tokenVal = plussym;
                strcpy(lexemeList[lexcount].varname, "+");
                lexcount++;
                break;
            //-
            case 1:
                lexemeList[lexcount].tokenVal = minussym;
                strcpy(lexemeList[lexcount].varname, "-");
                lexcount++;
                break;
            //*
            case 2:
                lexemeList[lexcount].tokenVal = multsym;
                strcpy(lexemeList[lexcount].varname, "*");
                lexcount++;
                break;
            //comment
            case 3:
                c = inputfile[++indx];
                peek = 1;
                if (c == '*')
                {
                    c = inputfile[++indx];
                    int loop = 1;
                    peek = 0;
                    while (loop)
                    {
                        if (c == '*')
                        {
                            c = inputfile[++indx];
                            if (c == '/')
                            {
                                loop = 0;
                            }
                        }
                        else
                        {
                            c = inputfile[++indx];
                        }
                    }
                }
                else
                { //just a slash. not a comment
                    lexemeList[lexcount].tokenVal = slashsym;
                    strcpy(lexemeList[lexcount].varname, "/");
                    lexcount++;
                }
                break;
            //)
            case 4:
                lexemeList[lexcount].tokenVal = lparentsym;
                strcpy(lexemeList[lexcount].varname, ")");
                lexcount++;
                break;
            //(
            case 5:
                lexemeList[lexcount].tokenVal = rparentsym;
                strcpy(lexemeList[lexcount].varname, "(");
                lexcount++;
                break;
            //=
            case 6:
                lexemeList[lexcount].tokenVal = eqsym;
                strcpy(lexemeList[lexcount].varname, "=");
                lexcount++;
                break;
            //,
            case 7:
                lexemeList[lexcount].tokenVal = commasym;
                strcpy(lexemeList[lexcount].varname, ",");
                lexcount++;
                break;
            //.
            case 8:
                lexemeList[lexcount].tokenVal = periodsym;
                strcpy(lexemeList[lexcount].varname, ".");
                lexcount++;
                break;
            //<>, >=, >
            case 9:
                c = inputfile[++indx];
                peek = 1;
                if (c == '>')
                {
                    lexemeList[lexcount].tokenVal = neqsym;
                    strcpy(lexemeList[lexcount].varname, "<>");
                    peek = 0;
                }
                else if (c == '=')
                {
                    lexemeList[lexcount].tokenVal = leqsym;
                    strcpy(lexemeList[lexcount].varname, "<=");
                    peek = 0;
                }
                else
                {
                    lexemeList[lexcount].tokenVal = lessym;
                    strcpy(lexemeList[lexcount].varname, "<");
                }
                lexcount++;
                break;
            //>=
            case 10:
                c = inputfile[++indx];
                peek = 1;
                if (c == '=')
                {
                    lexemeList[lexcount].tokenVal = geqsym;
                    strcpy(lexemeList[lexcount].varname, "=");
                    peek = 0;
                }
                else
                {
                    lexemeList[lexcount].tokenVal = gtrsym;
                    strcpy(lexemeList[lexcount].varname, ">=");
                }
                lexcount++;
                break;
            //;
            case 11:
                lexemeList[lexcount].tokenVal = semicolonsym;
                strcpy(lexemeList[lexcount].varname, ";");
                lexcount++;
                break;
            //:=
            case 12:
                c = inputfile[++indx];
                if (c == '=')
                {
                    lexemeList[lexcount].tokenVal = becomessym;
                    strcpy(lexemeList[lexcount].varname, ":=");
                    lexcount++;
                }
                else
                {
                    //printf("ERROR: Invalid symbols");
                    lexemeList[lexcount].errorType = 4;
                }
                break;
            default:
                //printf("ERROR: Invalid symbols");
                lexemeList[lexcount].errorType = 4;
                break;
            }
        }
        if (peek == 0)
        {
            c = inputfile[++indx];
        }
    }
    //print out lexeme list and token type
    i = 0;
    //printf("\nLEXEME TABLE:\nLEXEME\tTOKEN TYPE\n");
    for (i; i < lexcount; i++)
    {
        if (lexemeList[i].tokenVal == 3)
        {
            //printf("\n%d\t%d\t", lexemeList[i].numVal, lexemeList[i].tokenVal);
            if(lexemeList[i].errorType == 1){
                printf("\nERROR: Variable can't start with a number.");
            }else if(lexemeList[i].errorType == 2){
                printf("\nERROR: Number too long.");
            }else if(lexemeList[i].errorType == 3){
                printf("\nERROR: Name too long.");
            }else if(lexemeList[i].errorType == 4){
                printf("ERROR: Invalid symbols");
            }
        }
        else
        {
            //printf("\n%s\t%d\t", lexemeList[i].varname, lexemeList[i].tokenVal);
            if(lexemeList[i].errorType == 1){
                printf("\nERROR: Variable can't start with a number.");
            }else if(lexemeList[i].errorType == 2){
                printf("\nERROR: Number too long.");
            }else if(lexemeList[i].errorType == 3){
                printf("\nERROR: Name too long.");
            }else if(lexemeList[i].errorType == 4){
                printf("ERROR: Invalid symbols");
            }
        }
    }
    //prints lexeme list
    fprintf(output, "LEXEME LIST\n");
    for (i = 0; i < lexcount; i++)
    {
        fprintf(output, "%d ", lexemeList[i].tokenVal);
        list[i].type = lexemeList[i].tokenVal;
        if (lexemeList[i].tokenVal == 2)
        {
            fprintf(output, "%s ", lexemeList[i].varname);
            strcpy(list[i].name, lexemeList[i].varname);
        }
        else if (lexemeList[i].tokenVal == 3)
        {
           fprintf(output,"%d ", lexemeList[i].numVal);
            list[i].value = lexemeList[i].numVal;
        }
    }
    
    // printf("\nLex is analyzed\n");
   // fclose(input);
    fclose(output);
    return list;
}