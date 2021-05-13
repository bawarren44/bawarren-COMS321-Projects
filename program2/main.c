/**
* Brad Warren - bawarren@iastate.edu
* Patrick Shirazi - pshirazi@iastate.edu
* Joshua Beck - joshbeck@iastate.edu
*/

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <endian.h>
#include <inttypes.h>


#define ADD 0x458
#define ADDI 0x244 //0x488-0x489
#define AND 0x450
#define ANDI 0x248 //0x490-0x491
#define B 0x5 //0x0A0-0x0BF
#define Bcond 0x54 //0x2A0-0x2A7
#define BL 0x25 //0x4A0-0x4BF
#define BR 0x6B0
#define CBNZ 0xB5 //0x5A8-0x5AF
#define CBZ 0xB4 //0x5A0-0x5A7
#define DUMP 0x7FE
#define EOR 0x650
#define EORI 0x348 //0x690-0x691
#define HALT 0x7FF
#define LDUR 0x7C2
#define LDURB 0x1C2
#define LDURH 0x3C2
#define LDURSW 0x5C4
#define LSL 0x69B
#define LSR 0x69A
#define MUL 0x4D8 //0x4D8 / 0x1F <- shamt
#define ORR 0x550
#define ORRI 0x2C8 //0x590-0x591
#define PRNL 0x7FC
#define PRNT 0x7FD
#define SDIV 0x4D6 //0x4D6 / 0x02 <- shamt
#define SMULH 0x4DA
#define STUR 0x7C0
#define STURB 0x1C0
#define STURH 0x3C0
#define STURW 0x5C0
#define SUB 0x658
#define SUBI 0x344 //0x688-0x689
#define SUBIS 0x3C4 //0x788-0x789
#define SUBS 0x758
#define UDIV 0x4D6 //0x4D6 / 0x03 <- shamt
#define UMULH 0x4DE

typedef struct {
    uint32_t opcode11;
    uint32_t opcode10;
    uint32_t opcode8;
    uint32_t opcode6;
    uint32_t rm;
    uint32_t rn;
    uint32_t rd;
    uint32_t rt;
    uint32_t shamt;
    uint32_t aluImm;
    uint32_t dtAddr;
    uint32_t op;
    uint32_t brAddr;
    uint32_t condBrAddr;
} binary_instruction;

int64_t sign_extend(int64_t value, int bits)
{
    int i;
    if (value & (1 << (bits - 1))) { //If the value has a bit in the same digit as 1 shifted right by "bits - 1"
        for (i = 63; i >= bits; i--) {
            //This assigns value to be all bits in Value and all extra bits shifted right by i
            value |= 1ULL << i; // value = value | 1ULL << i
        }
    }

    return value;
}

void print_instruction(binary_instruction* instruction)
{
    printf("opcode11: %x\n", instruction->opcode11);
    printf("opcode10: %x\n", instruction->opcode10);
    printf("opcode8: %x\n", instruction->opcode8);
    printf("opcode6: %x\n", instruction->opcode6);
    printf("rm: %x\n", instruction->rm);
    printf("rn: %x\n", instruction->rn);
    printf("rd: %x\n", instruction->rd);
    printf("rt: %x\n", instruction->rt);
    printf("shamt: %x\n", instruction->shamt);
    printf("aluImm: %x\n", instruction->aluImm);
    printf("dtAddr: %x\n", instruction->dtAddr);
    printf("op: %x\n", instruction->op);
    printf("brAddr: %x\n", instruction->brAddr);
    printf("condBrAddr: %x\n", instruction->condBrAddr);
}

void print_instruction_formatted(binary_instruction* instruction)
{
    //R instructions
    switch(instruction->opcode11)
    {
	case ADD:
	    printf("ADD X%d, X%d, X%d\n", instruction->rd, instruction->rn, instruction->rm);
	    return;
	case AND:
            printf("AND X%d, X%d, X%d\n", instruction->rd, instruction->rn, instruction->rm);
            return;
	case BR:
            printf("BR X%d\n", instruction->rn);
            return;
	case DUMP:
            printf("DUMP\n");
            return;
	case EOR:
            printf("EOR X%d, X%d, X%d\n", instruction->rd, instruction->rn, instruction->rm);
            return;
	case HALT:
	    printf("HALT\n");
	    return;
	case LSL:
            printf("LSL X%d, X%d, #%d\n", instruction->rd, instruction->rn, instruction->shamt);
            return;
	case LSR:
            printf("LSR X%d, X%d, #%d\n", instruction->rd, instruction->rn, instruction->shamt);
            return;
	case MUL:
	    printf("MUL X%d, X%d, X%d\n", instruction->rd, instruction->rn, instruction->rm);
	    return;
	case ORR:
            printf("ORR X%d, X%d, X%d\n", instruction->rd, instruction->rn, instruction->rm);
            return;
	case PRNL:
            printf("PRNL\n");
            return;
	case PRNT:
            printf("PRNT X%d\n", instruction->rd);
            return;
	case SUB:
            printf("SUB X%d, X%d, X%d\n", instruction->rd, instruction->rn, instruction->rm);
            return;
	case SUBS:
            printf("SUBS X%d, X%d, X%d\n", instruction->rd, instruction->rn, instruction->rm);
            return;
    }
    //I instructions
    switch(instruction->opcode10)
    {
	case ADDI:
        printf("ADDI X%d, X%d, #%d\n", instruction->rd, instruction->rn, instruction->aluImm);
            return;
	case ANDI:
        printf("ANDI X%d, X%d, #%d\n", instruction->rd, instruction->rn, instruction->aluImm);
            return;
	case EORI:
        printf("EORI X%d, X%d, #%d\n", instruction->rd, instruction->rn, instruction->aluImm);
            return;
	case ORRI:
        printf("ORRI X%d, X%d, #%d\n", instruction->rd, instruction->rn, instruction->aluImm);
            return;
	case SUBI:
        printf("SUBI X%d, X%d, #%d\n", instruction->rd, instruction->rn, instruction->aluImm);
            return;
	case SUBIS:
        printf("SUBIS X%d, X%d, #%d\n", instruction->rd, instruction->rt, instruction->aluImm);
            return;
    }
    //D instructions
    switch(instruction->opcode11)
    {
	case LDUR:
        printf("LDUR X%d, [X%d, #%d]\n", instruction->rd, instruction->rn, instruction->dtAddr);
            return;
	case STUR:
        printf("STUR X%d, [X%d, #%d]\n", instruction->rd, instruction->rn, instruction->dtAddr);
            return;
    }
    //B instructions
    switch(instruction->opcode6)
    {
	case B:
        printf("B %d\n", instruction->brAddr);
            return;
	case BL:
        printf("BL %d\n", instruction->brAddr);
            return;
    }
    //CB instructions
    switch(instruction->opcode8)
    {
	case Bcond:
	    switch(instruction->rt)
	    {
            case 0:
		printf("B.EQ %d\n", instruction->condBrAddr); 
		break;
	    case 1:
		printf("B.NE %d\n", instruction->condBrAddr);
	       	break;
            case 2:
		printf("B.HS %d\n", instruction->condBrAddr);
		break;
            case 3:
		printf("B.LO %d\n", instruction->condBrAddr);
		break;
            case 4:
		printf("B.MI %d\n", instruction->condBrAddr);
		break;
            case 5:
		printf("B.PL %d\n", instruction->condBrAddr);
		break;
            case 6:
		printf("B.VS %d\n", instruction->condBrAddr);
		break;
            case 7:
		printf("B.VC %d\n", instruction->condBrAddr);
		break;
            case 8:
		printf("B.HI %d\n", instruction->condBrAddr);
		break;
            case 9:
		printf("B.LS %d\n", instruction->condBrAddr);
		break;
            case 10:
		printf("B.GE %d\n", instruction->condBrAddr);
		break;
            case 11:
		printf("B.LT %d\n", instruction->condBrAddr);
		break;
            case 12:
		printf("B.GT %d\n", instruction->condBrAddr);
		break;
            case 13:
		printf("B.LE %d\n", instruction->condBrAddr);
		break;
            }
	    return;
	case CBNZ:
        printf("CBNZ X%d, %d\n", instruction->rt, instruction->condBrAddr);
            return;
	case CBZ:
        printf("CBZ X%d, %d\n", instruction->rt, instruction->condBrAddr);
            return;
    }
}

void decode(unsigned int program, binary_instruction* instruction)
{
    instruction->opcode11 = program >> 21;
    instruction->opcode10 = program >> 22;
    instruction->opcode8 = program >> 24;
    instruction->opcode6 = program >> 26;
    instruction->rm = (program << 11) >> 27;
    instruction->rn = (program << 22) >> 27;
    instruction->rd = (program << 27) >> 27;
    instruction->rt = (program << 27) >> 27; //Same space as rd, but have for readability
    instruction->shamt = (program << 16) >> 26;
    instruction->aluImm = (((int)program) << 10) >> 20; //compiler dependent sign extension
    instruction->dtAddr = (((int)program) << 11) >> 23; //compiler dependent sign extension
    instruction->op = (program << 21) >> 30;
    instruction->brAddr = (((int)program) << 6) >> 6; //compiler dependent sign extension
    instruction->condBrAddr = (((int)program) << 8) >> 13; //compiler dependent sign extension

    print_instruction_formatted(instruction);
}

int main(int argc, char** argv) {
    int fd; //file descriptor
    struct stat buf; //file data
    unsigned int* program; //bytes of file
    binary_instruction* bprogram;

    fd = open(argv[1], O_RDONLY);
    fstat(fd, &buf);
    program = mmap(NULL, buf.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    bprogram = calloc(buf.st_size / 4, sizeof (*bprogram));
    int i;
    for (i = 0; i < (buf.st_size / 4); i++) {
        program[i] = be32toh(program[i]);
	decode(program[i], bprogram + i);
    }
    return 0;
}
