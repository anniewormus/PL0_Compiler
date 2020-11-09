#ifndef __PARSER_H
#define __PARSER_H

#include "lex.h"

#define MAX_LIST_SIZE 500

typedef struct symbol
{
	int kind;
	char name[12];
	int val;
	int level;
	int addr;
	int mark;
	
} symbol;

symbol* parse(lexeme* list);

#endif