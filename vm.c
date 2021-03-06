// COP3402
// 9.22.2020 - Andrea Wormus
// This code demonstrates a functioning P-Machine VM by reading in a
// file containing machine code and correctly executing it.
#include <stdio.h>
#include <stdlib.h>

#include "codegen.h"

#define MAX_STACK_HEIGHT 1000
#define MAX_CODE_LENGTH 500
int stack[1000] = {0};
int AR; //activation record location

FILE* output;

//method prototypes
int base(int, int);
void print(int[], int[], int, int, int);


void virtual_machine(instruction *code)
{
    output = fopen("vmout.txt", "w");
    //default PM/0 values
    int SP = MAX_STACK_HEIGHT;
    int BP = SP - 1;
    int PC = 0;
    int RF[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    //allocate memory for array of structs storing file contents

    fprintf(output, "\t\t\t\tPC\tBP\tSP\n");
    fprintf(output, "Initial Values: ");
    print(RF, stack, PC, BP, SP);

    //populating the instruction array with codes
    int counter = 0;
    while (counter < 500)
    {
        counter++;
        int R = code[PC].r;
        int L = code[PC].l;
        int M = code[PC].m;

        switch (code[PC].opcode)
        {
        //LIT - loads a constant value M into register R
        case 1:
            fprintf(output, "\n\n%d LIT %d %d %d", PC, R, L, M);
            RF[R] = M;
            PC++;
            print(RF, stack, PC, BP, SP);
            break;
        //RTN - Returns from a subroutine and resotre the caller environment
        case 2:
            fprintf(output, "\n\n%d RTN %d %d %d", PC, R, L, M);
            SP = BP + 1;
            BP = stack[SP - 2];
            PC = stack[SP - 3];
            //PC++;
            print(RF, stack, PC, BP, SP);
            break;
        //LOD - Load value into a selected register from the stack location at offset M from L
        case 3:
            fprintf(output, "\n\n%d LOD %d %d %d", PC, R, L, M);
            RF[R] = stack[base(L, BP) - M];
            print( RF, stack, PC, BP, SP);
            break;
        //STO - Sotre value from a selected reg. in the stack location at offset M from L
        case 4:
            fprintf(output, "\n\n%d STO %d %d %d", PC, R, L, M);
            stack[base(L, BP) - M] = RF[R];
            PC++;
            print(RF, stack, PC, BP, SP);
            break;
        //CAL - Call procedure at code index M
        case 5:
            fprintf(output, "\n\n%d CAL %d %d %d", PC, R, L, M);
            PC++;
            stack[SP - 1] = base(L, BP);
            AR = SP - 1;
            stack[SP - 2] = BP;
            stack[SP - 3] = PC;
            BP = SP - 1;
            PC = M;
            print(RF, stack, PC, BP, SP);
            break;
        //INC - Allocate M memory words.
        case 6:
            fprintf(output, "\n\n%d INC %d %d %d", PC, R, L, M);
            SP = SP - M;
            PC++;
            print(RF, stack, PC, BP, SP);
            break;
        //JMP - Jump to instruction register M
        case 7:
            fprintf(output, "\n\n%d JMP %d %d %d", PC, R, L, M);
            PC = M;
            print(RF, stack, PC, BP, SP);
            break;
        //JPC - Jump to instruction M if R = 0
        case 8:
            fprintf(output, "\n\n%d JPC %d %d %d", PC, R, L, M);
            if (RF[R] == 0)
            {
                PC = M;
            }
            print(RF, stack, PC, BP, SP);
            break;
        //SYS
        case 9:
            switch (M)
            {
            //Write reg. to screen
            case 1:
                fprintf(output, "%d", RF[R]);
            //Read in input from the user and store it in a reg.
            case 2:
                printf("enter a numbr to store in a register: ");
                if(scanf("%d", &RF[R]) > 0 );
            //End of program
            case 3:
                fprintf(output, "\n\n%d SYS %d %d %d", PC, R, L, M);
                PC++;
                print(RF, stack, PC, BP, SP);
                exit(0);
            }
        //NEG
        case 10:
            RF[R] = -RF[R];
        //ADD
        case 11:
            RF[R] = RF[L] + RF[M];
        //SUB
        case 12:
            RF[R] = RF[L] - RF[M];
        //MUL
        case 13:
            RF[R] = RF[L] * RF[M];
        //DIV
        case 14:
            RF[R] = RF[L] / RF[M];
        //ODD
        case 15:
            if (RF[R] % 2 == 1)
                RF[R] = 1;
            else
                RF[R] = 0;
        //MOD
        case 16:
            RF[R] = RF[L] % RF[M];
        //EQL
        case 17:
            RF[R] = RF[L] == RF[M];
        //NEQ
        case 18:
            RF[R] = RF[L] != RF[M];
        //LSS
        case 19:
            RF[R] = RF[L] < RF[M];
        //LEQ
        case 20:
            RF[R] = RF[L] <= RF[M];
        //GTR
        case 21:
            RF[R] = RF[L] > RF[M];
        //GEQ
        case 22:
            RF[R] = RF[L] >= RF[M];
        }
    }

    // Close the file
    fclose(output);
    return;
}
int base(int l, int base)
{
    int b1;
    b1 = base;
    while (l > 0)
    {
        b1 = stack[b1];
        l--;
    }
    return b1;
}
//print function
void print(int RF[], int stack[], int PC, int BP, int SP)
{
    int i;
    fprintf(output, "\t\t\t\t%d\t%d\t%d\t", PC, BP, SP);
    fprintf(output, "\nRegisters: ");
    for (i = 0; i < 8; i++)
    {
        fprintf(output, "%d ", RF[i]);
    }
    fprintf(output, "\nStack: ");
    //prints contents of stack as well as activation record
    for (i = MAX_STACK_HEIGHT - 1; i >= SP; i--)
    {
        if (i == AR)
            fprintf(output, "|");
        fprintf(output, "%d ", stack[i]);
    }
}