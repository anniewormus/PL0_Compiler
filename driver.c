#include <stdio.h>
#include <string.h>
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
	for(i = 0; i < argc; i++){
		if(strcmp(argv[i], "-l") == 0){
			printf("lflag set\n");
			lflag = 1;
		}
		if(strcmp(argv[i], "-v") == 0){
			printf("vflag set\n");
			vflag = 1;
		}
		if(strcmp(argv[i], "-a") == 0){
			printf("aflag set\n");
			aflag = 1;
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
	
	lexeme *list = lex_analyze(inputfile);
	symbol *table = parse(list);
	instruction *code = generate_code(table, list);
	virtual_machine(code);

	if(lflag == 1){
		printf("SCANNER OUTPUT:\n");
		FILE *lexfile = fopen("lexout.txt", "r");
		while((c = fgetc(lexfile)) != EOF){
            printf("%c", c);
        }
        printf("\n\n");
        fclose(lexfile);
	}
	if(aflag == 1){
		printf("PARSER/CODEGEN OUTPUT:\n");
		FILE *codefile = fopen("codegenout.txt", "r");
		while((c = fgetc(codefile)) != EOF){
            printf("%c", c);
        }
        printf("\n\n");
        fclose(codefile);
	}
	if(vflag == 1){
		printf("VIRTUAL MACHINE OUTPUT:\n");
		FILE *vmfile = fopen("vmout.txt", "r");
		while((c = fgetc(vmfile)) != EOF){
            printf("%c", c);
        }
        printf("\n\n");
        fclose(vmfile);
	}
	
	return 0;
}