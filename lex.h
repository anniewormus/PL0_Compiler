#define __LEX_H

typedef struct lexeme
{
	char *name;
	int value;
	int type;
	
} lexeme;

typedef enum
{
    nulsym = 1,
    identsym,
    numbersym,
    plussym,
    minussym,
    multsym,
    slashsym,
    oddsym,
    eqsym,
    neqsym,
    lessym,
    leqsym,
    gtrsym,
    geqsym,
    lparentsym,
    rparentsym,
    commasym,
    semicolonsym,
    periodsym,
    becomessym,
    beginsym,
    endsym,
    ifsym,
    thensym,
    whilesym,
    dosym,
    callsym,
    constsym,
    varsym,
    procsym,
    writesym,
    readsym,
    elsesym
} token_type;

//token structure
typedef struct
{
    token_type tokenVal;
    int kind;
    int numVal;
    char varname[10]; //in slides
    int errorType;
} tokenStruct;

extern tokenStruct lexemeList[3000]; //array of token structs
extern int lexcount = 0;   //keeps track of array location

