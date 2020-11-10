#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"
#include "vm.h"

int main(int argc, char **argv)
{
	//reads in console commands for what the user wants to see
	int lflag, aflag, vflag;
	int i;
	for(i = 0; i < 3; i++){
		if(strcmp(argv[i], "-l") == 0){
			printf("lflag set\n");
			lflag = 1;
		}
		if(strcmp(argv[i], "-a") == 0){
			printf("aflag set\n");
			aflag = 1;
		}
		if(strcmp(argv[i], "-v") == 0){
			printf("vflag set\n");
			vflag = 1;
		}
	}
//get file name from user input
    char fileName[25];
    printf("What is your input file called? ");
    if(scanf("%s", fileName) > 0);

	if (fileName == "")
		printf("error : please include the file name");
	
	FILE *ifp = fopen(fileName, "r");
	
	char *inputfile = malloc(500 * sizeof(char));
	char c = fgetc(ifp);
	i = 0;
	while (1)
	{
		inputfile[i++] = c;
		c = fgetc(ifp);
		if (c == EOF)
			break;
	}
	inputfile[i] = '\0';
	
	printf("%s\n", inputfile);
	
	lexeme *list = lex_analyze(inputfile);
	symbol *table = parse(list);
	printf("done w parse\n");
	instruction *code = generate_code(table, list);
	printf("done w code gen\n");
	virtual_machine(code);
	printf("done w vm\n");

	if(lflag == 1){
		printf("Lex Output:\n");
		FILE *lexfile = fopen("lexout.txt", "r");
		while((c = fgetc(lexfile)) != EOF){
            printf("%c", c);
        }
        printf("\n\n");
        fclose(ifp);
	}
	
	return 0;
}