#include <stdio.h>

#define MOVE 0
#define ADD 1
#define SUB 2
#define BRA 3
#define CMP 4
#define BEQ 5
#define BNE 6
#define EXG 7
#define EMEM 8
#define STOP 15

void main() {

    unsigned short int PC = 0;                      // program counter
    unsigned short int D0 = 0;                      // data register
    unsigned short int D1 = 0;
    unsigned short int A0 = 0;                      // address register
    unsigned short int CCR = 0;                     // condition code register
    unsigned short int MAR;                         // memory address register
    unsigned short int MBR;                         // memory buffer register
    unsigned short int IR;                          // Instruction register
    unsigned short int operand;                     // the 8-bit operand from the IR
    unsigned short int source;                      // source operand
    unsigned short int destination;                 // destination value
    unsigned short int opcode;                      // the 4-bit op-code from the IR
    unsigned short int amode;                 // the 2-bit addressing mode
    unsigned short int direction;                   // the 1-bit direction flag
    unsigned short int memory[256];                 // the memory
    unsigned short int run = 1;                     // execution of program

    /*
     * Instruction format
     * 7   6   5   4   3   2   1   0
     * bits 3-0: 4 bit operand
     * bit 4: one bit addressing mode
     * bits 7-5: three bit instruction code
     * */

	memory[0] = 0x05 ;		// opcode
	memory[1] = 0x00 ;		// operand

	memory[2] = 0x05 ;		// opcode
	memory[3] = 0x4F ;		// operand

	memory[4] = 0xF0 ;		// opcode
	memory[5] = 0x00 ;		// operand

    printf("START\n\n");


	while (run) {
        /*===========================================================================================================*/
        /*two fetch cycles*/
        MAR = PC;
        PC++;
        MBR = memory[MAR];
        IR = MBR;
        opcode = IR;

        MAR = PC;
        PC++;
        MBR = memory[MAR];
        IR = MBR;
        operand = IR;
        /*===========================================================================================================*/

        amode = opcode & 0x03;					//extract address mode bits
        direction = (opcode & 0x04) >> 2;		//extract direction bit
        opcode = opcode >> 4;                   //extract opcode bits

        /*===========================================================================================================*/
        /*use address mode to get source data*/
        switch (amode) {
            case 0: {
                source = memory[operand];
                break;
            }                                       //Absolute
            case 1: {
                source = operand;
                break;
            }                                       //Literal
            case 2: {
                source = memory[A0 + operand];
                break;
            }                                       //Indexed
            case 3: {
                source = memory[PC + operand];
                break;
            }                                       //PC relative
        }
        /*===========================================================================================================*/
        /* now execute the instruction */
        switch (opcode) {
            case MOVE: {
                if (direction == 0)	destination = D0;
                else		D0 = source;
                if (D0 == 0)	CCR = 1;
                else		CCR = 0;
                break;
            }
            case ADD: {
                if (direction == 0) {
                    destination = D0 + source;
                    if (destination == 0)	CCR = 1;
                    else		CCR = 0;
                }
				else {
                    D0 = D0 + source;
                    if (D0 == 0)	CCR = 1;
                    else		CCR = 0;
                }
                break;
            }
            case SUB: {
                if (direction == 0) {
                    destination = D0 - source;
                    if (destination == 0)	CCR = 1;
                    else		CCR = 0;
                }
				else {
                    D0 = D0 - source;
                    if (D0 == 0)	CCR = 1;
                    else		CCR = 0;
                }
                break;
            }
            case BRA: {
                if (amode == 0)		PC = operand;
                if (amode == 1)		PC = PC + operand;
                break;
            }
            case CMP: {
                MBR = D0 - source;
                if (MBR == 0)	CCR = 1;
                else		CCR = 0;
                break;
            }
            case BEQ: {
                if (CCR == 1) {
                    if (amode == 0)	PC = operand;
                    if (amode == 1)	PC = PC + operand;
                }
                break;
            }
            case BNE: {
                if (CCR != 1) {
                    if (amode == 0)	PC = operand;
                    if (amode == 1)	PC = PC + operand;
                }
                break;
            }
            case EXG: {
                MBR = D0;
                D0 = A0;
                A0 = MBR;
                break;
            }
            case STOP: {
                run = 0;
                break;
            }
            case EMEM:{
                MBR=PC+4;
                IR=MBR;
                MAR=IR;
                memory[MAR]=memory[D0*D1];
                destination=D0+D1;
                D0=D0*4;
                D1=D1/2;
                D0=D0+memory[D1];
                MBR=D0;
                IR=MBR;
                MAR=IR;
                memory[destination]=memory[MAR];
            }
        }
        /*===========================================================================================================*/
        /* save result in memory if register to memory */
        if (direction == 0) {
            switch (amode) {
                case 0: {	// Absolute
                    memory[operand] = destination;
                    break;
                }
                case 1: {	// Literal
                    break;
                }
                case 2: {	// Indexed
                    memory[A0 + operand] = destination;
                    break;
                }
                case 3: {	// PC relative
                    memory[PC + operand] = destination;
                    break;
                }
            }
        }	// end switch

		printf("D0=%X , A0=%X , PC=%X , CCR=%X \n",D0,A0,PC,CCR);

    }	// end while

	printf("\n\nCPU STOPPED \n");
}	// end main
