#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#define MAXCHAR 1000

typedef struct {
    int pc;
    uint64_t* registers;
    int stackSize;
    uint8_t* stack;
    int memorySize;
    uint8_t* memory;
    int EQ;
    int GE;
    int GT;
    int HI;
    int HS;
    int LE;
    int LO;
    int LS;
    int LT;
    int MI;
    int NE;
    int PL;
    int VC;
    int VS;
} machine_state;

typedef struct {
    uint32_t opcode;
    uint32_t rm;
    uint32_t rn;
    uint32_t rd;
    uint32_t rt;
    uint64_t shamt;
    uint64_t aluImm;
    uint64_t dtAddr;
    uint32_t op;
    uint64_t brAddr;
    uint64_t condBrAddr;
} binary_instruction;

char printable_char(uint8_t c)
{
    return isprint(c) ? c : '.';
}

void hexdump(int8_t* start, size_t size)
{
    size_t i;

    for(i = 0; i < size - (size % 16); i+= 16)
    {
	printf(
	    "%08x "
	    " %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx "
	    " %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx "
	    " |%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c|\n",
	    (int32_t) i,
	    start[i+0], start[i+1], start[i+2], start[i+3],
	    start[i+4], start[i+5], start[i+6], start[i+7],
	    start[i+8], start[i+9], start[i+10], start[i+11],
	    start[i+12], start[i+13], start[i+14], start[i+15],
	    printable_char(start[i+0]), printable_char(start[i+1]),
	    printable_char(start[i+2]), printable_char(start[i+3]),
	    printable_char(start[i+4]), printable_char(start[i+5]),
	    printable_char(start[i+6]), printable_char(start[i+7]),
	    printable_char(start[i+8]), printable_char(start[i+9]),
	    printable_char(start[i+10]), printable_char(start[i+11]),
	    printable_char(start[i+12]), printable_char(start[i+13]),
	    printable_char(start[i+14]), printable_char(start[i+15]));
    }

    printf("%08x\n", (int32_t) size);
}

char* registerString(int index, uint64_t value)
{
    char* string = malloc(50*sizeof(char));

    if(index > 9)
    {
	sprintf(string, "X%d: %#018llx (%lld)", index, value, value);
    }
    else
    {
        sprintf(string, "X%d:  %#018llx (%lld)", index, value, value);
    }

    return string;
}

void printRegisters(uint64_t* registers)
{
    int i;
    for(i = 0; i < 32; i++)
    {
	switch(i)
	{
	    case 16:
		printf("(IP0) %s\n", registerString(i, registers[i]));
		break;
	    case 17:
		printf("(IP1) %s\n", registerString(i, registers[i]));
		break;
	    case 28:
		printf(" (SP) %s\n", registerString(i, registers[i]));
		break;
	    case 29:
		printf(" (FP) %s\n", registerString(i, registers[i]));
		break;
	    case 30:
		printf(" (LR) %s\n", registerString(i, registers[i]));
		break;
	    case 31:
		printf("(XZR) %s\n", registerString(i, registers[i]));
		break;
	    default:
		printf("      %s\n", registerString(i, registers[i]));
	}
    }
}

void printAssembly(binary_instruction* instList, int numInst, int printLine)
{
    int i;
    binary_instruction* inst;
    for(i = 0; i < numInst; i++)
    {
	inst = &instList[i];
	if(i == printLine)
	{
	    printf("-->");
	}
	else
	{
	    printf("   ");
	}
    	switch(inst->opcode)
    	{
            case 1112: //ADD
	        printf("ADD X%d, X%d, X%d\n", inst->rd, inst->rn, inst->rm);
	        break;
	    case 580: //ADDI
	        printf("ADDI X%d, X%d, #%lld\n", inst->rd, inst->rn, inst->aluImm);
	        break;
	    case 1104: //AND
	    	printf("AND X%d, X%d, X%d\n", inst->rd, inst->rn, inst->rm);
	    	break;
	    case 584: //ANDI
	    	printf("ANDI X%d, X%d, #%lld\n", inst->rd, inst->rn, inst->aluImm);
	    	break;
	    case 5: //B
	    	printf("B %lld\n", inst->brAddr);
	    	break;
	    case 84: //B.cond
	    	switch(inst->rt)
	    	{
		    case 0:
		    	printf("B.EQ %lld\n", inst->condBrAddr);
		    	break;
		    case 1:
		    	printf("B.NE %lld\n", inst->condBrAddr);
		    	break;
		    case 2:
		    	printf("B.HS %lld\n", inst->condBrAddr);
		    	break;
		    case 3:
		      	printf("B.LO %lld\n", inst->condBrAddr);
		    	break;
		    case 4:
		    	printf("B.MI %lld\n", inst->condBrAddr);
		    	break;
		    case 5:
		    	printf("B.PL %lld\n", inst->condBrAddr);
		    	break;
		    case 6:
		    	printf("B.VS %lld\n", inst->condBrAddr);
		    	break;
		    case 7:
		    	printf("B.VC %lld\n", inst->condBrAddr);
		    	break;
		    case 8:
		    	printf("B.HI %lld\n", inst->condBrAddr);
		    	break;
		    case 9:
		    	printf("B.LS %lld\n", inst->condBrAddr);
		    	break;
		    case 10:
		    	printf("B.GE %lld\n", inst->condBrAddr);
		    	break;
		    case 11:
		    	printf("B.LT %lld\n", inst->condBrAddr);
		    	break;
		    case 12:
		    	printf("B.GT %lld\n", inst->condBrAddr);
		    	break;
		    case 13:
		    	printf("B.LE %lld\n", inst->condBrAddr);
		    	break;
	    	}
	    	break;
	    case 37: //BL
	    	printf("BL %lld\n", inst->brAddr);
	    	break;
	    case 1712: //BR
	    	printf("BR X%d\n", inst->rn);
	    	break;
	    case 181: //CBNZ
	    	printf("CBNZ X%d, %lld\n", inst->rt, inst->condBrAddr);
	    	break;
	    case 180: //CBZ
	    	printf("CBZ X%d, %lld\n", inst->rt, inst->condBrAddr);
	    	break;
	    case 1616: //EOR
	    	printf("EOR X%d, X%d, X%d\n", inst->rd, inst->rn, inst->rm);
	    	break;
	    case 840: //EORI
	    	printf("EORI X%d, X%d, #%lld\n", inst->rd, inst->rn, inst->aluImm);
	    	break;
	    case 1986: //LDUR
	    	printf("LDUR X%d, [X%d, #%lld]\n", inst->rd, inst->rn, inst->dtAddr);
	    	break;
	    case 1691: //LSL
	    	printf("LSL X%d, X%d, #%lld\n", inst->rd, inst->rn, inst->shamt);
	    	break;
	    case 1690: //LSR
	    	printf("LSR X%d, X%d, #%lld\n", inst->rd, inst->rn, inst->shamt);
	    	break;
	    case 1360: //ORR
	    	printf("ORR X%d, X%d, X%d\n", inst->rd, inst->rn, inst->rm);
	    	break;
	    case 712: //ORRI
	    	printf("ORRI X%d, X%d, #%lld\n", inst->rd, inst->rn, inst->aluImm);
	    	break;
	    case 1984: //STUR
	    	printf("STUR X%d, [X%d #%lld]\n", inst->rd, inst->rn, inst->dtAddr);
	    	break;
	    case 1624: //SUB
	    	printf("SUB X%d, X%d, X%d\n", inst->rd, inst->rn, inst->rm);
	    	break;
	    case 836: //SUBI
	    	printf("SUBI X%d, X%d, #%lld\n", inst->rd, inst->rn, inst->aluImm);
	    	break;
	    case 964: //SUBIS
	    	printf("SUBIS X%d, X%d, #%lld\n", inst->rd, inst->rn, inst->aluImm);
	    	break;
	    case 1880: //SUBS
	    	printf("SUBS X%d, X%d, X%d\n", inst->rd, inst->rn, inst->rm);
	    	break;
	    case 1240: //MUL
	    	printf("MUL X%d, X%d, X%d\n", inst->rd, inst->rn, inst->rm);
	    	break;
	    case 2045: //PRNT
	    	printf("PRNT X%d\n", inst->rd);
	    	break;
	    case 2044: //PRNL
	    	printf("PRNL\n");
	    	break;
	    case 2046: //DUMP
	    	printf("DUMP\n");
	    	break;
	    case 2047: //HALT
	    	printf("HALT\n");
	    	break;
    	}
    }
}

void dump(binary_instruction* instList, int numInst, machine_state* m)
{
    printf("Registers:\n");
    printRegisters(m->registers);

    printf("\nStack:\n");
    hexdump(m->stack, 512);

    printf("\nMain Memory:\n");
    hexdump(m->memory, 4096);

    printf("\nAssembly:\n");
    printAssembly(instList, numInst, m->pc);
}

void halt(binary_instruction* instList, int numInst, machine_state* m, int error, uint64_t addr)
{
    dump(instList, numInst, m);	
    if(error)
    {
        printf("Address: %#018llx out of bounds\n", addr);
    }
    else
    {
	printf("Halt executed, ending program");
    }
    exit(1);
}

void setFlags(int64_t S, machine_state* m)
{
    m->EQ = S == 0 ? 1 : 0;
    m->GE = S >= 0 ? 1 : 0;
    m->GT = S >  0 ? 1 : 0;
    m->HI = S >  0 ? 1 : 0;
    m->HS = S >= 0 ? 1 : 0;
    m->LE = S <= 0 ? 1 : 0;
    m->LO = S <  0 ? 1 : 0;
    m->LS = S <= 0 ? 1 : 0;
    m->LT = S <  0 ? 1 : 0;
    m->MI = S <  0 ? 1 : 0;
    m->NE = S != 0 ? 1 : 0;
    m->PL = S >= 0 ? 1 : 0;
    m->VC = 0;
    m->VS = 0;
}

void emulate(binary_instruction* instList, int numInst, machine_state* m)
{
    binary_instruction* inst;
    uint64_t addr;
    uint64_t ldurtmp;
    int64_t S;

    while(m->pc < numInst)
    {
	inst = &instList[m->pc];
        switch(inst->opcode)
    	{
	    case 1112: //ADD
	    	m->registers[inst->rd] = m->registers[inst->rn] + m->registers[inst->rm];
	    	m->pc++;
	    	break;
	    case 580: //ADDI
		m->registers[inst->rd] = m->registers[inst->rn] + inst->aluImm;
	    	m->pc++;
	    	break;
	    case 1104: //AND
	    	m->registers[inst->rd] = m->registers[inst->rn] & m->registers[inst->rm];
	    	m->pc++;
	    	break;
	    case 584: //ANDI
	    	m->registers[inst->rd] = m->registers[inst->rn] & inst->aluImm;
	    	m->pc++;
	    	break;
	    case 5: //B
	    	m->pc = m->pc + inst->brAddr;
	    	break;
	    case 84: //B.cond
		switch(inst->rt)
		{
		    case 0:
			if(m->EQ)
			{
			    m->pc = m->pc + inst->condBrAddr;
			}
			else{
			    m->pc++;
			}
			break;
		    case 1:
			if(m->NE)
			{
			    m->pc = m->pc + inst->condBrAddr;
			}
			else{
			    m->pc++;
			}
			break;
		    case 2:
			if(m->HS)
			{
			    m->pc = m->pc + inst->condBrAddr;
			}
			else{
			    m->pc++;
			}
			break;
		    case 3:
			if(m->LO)
			{
			    m->pc = m->pc + inst->condBrAddr;
			}
			else{
			    m->pc++;
			}
			break;
		    case 4:
			if(m->MI)
			{
			    m->pc = m->pc + inst->condBrAddr;
			}
			else{
			    m->pc++;
			}
			break;
		    case 5:
			if(m->PL)
			{
			    m->pc = m->pc + inst->condBrAddr;
			}
			else{
			    m->pc++;
			}
			break;
		    case 6:
			if(m->VS)
			{
			    m->pc = m->pc + inst->condBrAddr;
			}
			else{
			    m->pc++;
			}
			break;
		    case 7:
			if(m->VC)
			{
			    m->pc = m->pc + inst->condBrAddr;
			}
			else{
			    m->pc++;
			}
			break;
		    case 8:
			if(m->HI)
			{
			    m->pc = m->pc + inst->condBrAddr;
			}
			else{
			    m->pc++;
			}
			break;
		    case 9:
			if(m->LS)
			{
			    m->pc = m->pc + inst->condBrAddr;
			}
			else{
			    m->pc++;
			}
			break;
		    case 10:
			if(m->GE)
			{
			    m->pc = m->pc + inst->condBrAddr;
			}
			else{
			    m->pc++;
			}
			break;
		    case 11:
			if(m->LT)
			{
			    m->pc = m->pc + inst->condBrAddr;
			}
			else{
			    m->pc++;
			}
			break;
		    case 12:
			if(m->GT)
			{
			    m->pc = m->pc + inst->condBrAddr;
			}
			else{
			    m->pc++;
			}
			break;
		    case 13:
			if(m->LE)
			{
			    m->pc = m->pc + inst->condBrAddr;
			}
			else{
			    m->pc++;
			}
			break;
		}
	    	break;
	    case 37: //BL
		m->registers[30] = m->pc + 1; //Set LR to next instruction
	    	m->pc = m->pc + inst->brAddr;
	    	break;
	    case 1712: //BR
	    	m->pc = m->registers[inst->rt];
	        break;
	    case 181: //CBNZ
	    	if(m->registers[inst->rt] != 0){
		    m->pc = m->pc + inst->condBrAddr;
	        }
	        else{
			    m->pc++;
			}
	    	break;
	    case 180: //CBZ
	    	if(m->registers[inst->rt] == 0){
		    m->pc = m->pc + inst->condBrAddr;
	    	}
	    	else{
			    m->pc++;
			}
	    	break;
	    case 1616: //EOR
	    	m->registers[inst->rd] = (m->registers[inst->rn]) ^ (m->registers[inst->rm]);
	    	m->pc++;
	    	break;
	    case 840: //EORI
	    	m->registers[inst->rd] = (m->registers[inst->rn]) ^ (inst->aluImm);
	    	m->pc++;
	    	break;
	    case 1986: //LDUR
		addr = m->registers[inst->rn] + inst->dtAddr;
		ldurtmp = 0;
	    	if(inst->rn == 28 || inst->rn == 29) //FP or SP
		{
		    if(addr + 7 >= m->stackSize || addr < 0)
		    {
			halt(instList, numInst, m, 1, addr);
		    }
		    
		    m->registers[inst->rt] = (((uint64_t)m->stack[addr + 0]) << 56) | (((uint64_t)m->stack[addr + 1]) << 48) |
			                     (((uint64_t)m->stack[addr + 2]) << 40) | (((uint64_t)m->stack[addr + 3]) << 32) |
					     (((uint64_t)m->stack[addr + 4]) << 24) | (((uint64_t)m->stack[addr + 5]) << 16) |
					     (((uint64_t)m->stack[addr + 6]) << 8)  | (((uint64_t)m->stack[addr + 7]));

		}
		else
		{
		    if(addr + 7 >= m->memorySize || addr < 0)
		    {
			halt(instList, numInst, m, 1, addr);
		    }
		    m->registers[inst->rt] = (((uint64_t)m->memory[addr + 0]) << 56) | (((uint64_t)m->memory[addr + 1]) << 48) |
			                     (((uint64_t)m->memory[addr + 2]) << 40) | (((uint64_t)m->memory[addr + 3]) << 32) |
					     (((uint64_t)m->memory[addr + 4]) << 24) | (((uint64_t)m->memory[addr + 5]) << 16) |
					     (((uint64_t)m->memory[addr + 6]) << 8)  | (((uint64_t)m->memory[addr + 7]));

		}
		m->pc++;
	    	break;
	    case 1691: //LSL
	    	m->registers[inst->rd] = m->registers[inst->rn] << inst->shamt;
	    	m->pc++;
	    	break;
	    case 1690: //LSR
	    	m->registers[inst->rd] = ((int64_t)m->registers[inst->rn]) >> inst->shamt;
	    	m->pc++;
	    	break;
	    case 1360: //ORR
	    	m->registers[inst->rd] = m->registers[inst->rn] | m->registers[inst->rm];
	    	m->pc++;
	    	break;
	    case 712: //ORRI
	    	m->registers[inst->rd] = m->registers[inst->rn] | inst->aluImm;
	    	m->pc++;
	    	break;
	    case 1984: //STUR
		addr = m->registers[inst->rn] + inst->dtAddr;	
		if(inst->rn == 28 || inst->rn == 29) //FP or SP
		{
		    if(addr + 7 >= m->stackSize || addr < 0)
		    {
			halt(instList, numInst, m, 1, addr);
		    }

		    m->stack[addr + 0] = ((m->registers[inst->rt] >> 56) & 0xFF);
		    m->stack[addr + 1] = ((m->registers[inst->rt] >> 48) & 0xFF);
		    m->stack[addr + 2] = ((m->registers[inst->rt] >> 40) & 0xFF);
		    m->stack[addr + 3] = ((m->registers[inst->rt] >> 32) & 0xFF);
		    m->stack[addr + 4] = ((m->registers[inst->rt] >> 24) & 0xFF);
		    m->stack[addr + 5] = ((m->registers[inst->rt] >> 16) & 0xFF);
		    m->stack[addr + 6] = ((m->registers[inst->rt] >> 8) & 0xFF);
		    m->stack[addr + 7] = ((m->registers[inst->rt]) & 0xFF);
		}
		else
		{
		    if(addr + 7 >= m->memorySize || addr < 0)
		    {
			halt(instList, numInst, m, 1, addr);
		    }
 
		    m->memory[addr + 0] = ((m->registers[inst->rt] >> 56) & 0xFF);
		    m->memory[addr + 1] = ((m->registers[inst->rt] >> 48) & 0xFF);
		    m->memory[addr + 2] = ((m->registers[inst->rt] >> 40) & 0xFF);
		    m->memory[addr + 3] = ((m->registers[inst->rt] >> 32) & 0xFF);
		    m->memory[addr + 4] = ((m->registers[inst->rt] >> 24) & 0xFF);
		    m->memory[addr + 5] = ((m->registers[inst->rt] >> 16) & 0xFF);
		    m->memory[addr + 6] = ((m->registers[inst->rt] >> 8) & 0xFF);
		    m->memory[addr + 7] = ((m->registers[inst->rt]) & 0xFF);
		}
	    	m->pc++;
	    	break;
	    case 1624: //SUB
		m->registers[inst->rd] = m->registers[inst->rn] - m->registers[inst->rm];
	    	m->pc++;
	    	break;
	    case 836: //SUBI
		m->registers[inst->rd] = m->registers[inst->rn] - inst->aluImm;
	    	m->pc++;
	    	break;
	    case 964: //SUBIS
		m->registers[inst->rd] = m->registers[inst->rn] - inst->aluImm;
		setFlags(m->registers[inst->rd], m);
	    	m->pc++;
	    	break;
	    case 1880: //SUBS
		m->registers[inst->rd] = m->registers[inst->rn] - m->registers[inst->rm];
		setFlags(m->registers[inst->rd], m);
	    	m->pc++;
	    	break;
	    case 1240: //MUL
		m->registers[inst->rd] = (m->registers[inst->rn]) * (m->registers[inst->rm]);
	    	m->pc++;
	    	break;
	    case 2045: //PRNT
		printf("%s\n", registerString(inst->rd, m->registers[inst->rd]));
	    	m->pc++;
	    	break;
	    case 2044: //PRNL
	    	printf("\n");
	    	m->pc++;
	        break;
	    case 2046: //DUMP
	    	dump(instList, numInst, m);
	    	m->pc++;
	        break;
	    case 2047: //HALT
		halt(instList, numInst, m, 0, 0);
	    	return; //ends the program
    	}
	m->registers[31] = 0; //discard any changes to XZR
    }
}

binary_instruction* parseTextFile(binary_instruction *instArr, int arrSize, int* instCounter)
{
    int arrCounter = 0;
    char str[MAXCHAR];
    FILE *instFile;
    instFile = fopen("decimal.txt","r");
    while(fgets(str,MAXCHAR,instFile) != NULL)
    {
        if(arrCounter >= arrSize){
            arrSize += arrSize;
            instArr = (binary_instruction*)realloc(instArr,arrSize * sizeof(binary_instruction));
        }
        //instArr[arrCounter].opcode = arrCounter;
        //printf("%d\n", instArr[arrCounter].opcode);
//        printf("%s", str);
        char* token = strtok(str, ", ");
        int oddCount = 1;
        while( token != NULL ) {
            if(oddCount%2 == 0){
//                printf( " %s\n", token );//printing each token
                switch(oddCount){
                    case 2: instArr[arrCounter].opcode = (uint32_t)atoi(token); break;
                    case 4: instArr[arrCounter].rd = (uint32_t)atoi(token); break;
                    case 6: instArr[arrCounter].rt = (uint32_t)atoi(token); break;
                    case 8: instArr[arrCounter].rn = (uint32_t)atoi(token); break;
                    case 10: instArr[arrCounter].rm = (uint32_t)atoi(token); break;
                    case 12: instArr[arrCounter].shamt = (uint64_t)atoi(token); break;
                    case 14: instArr[arrCounter].aluImm = (uint64_t)atoi(token); break;
                    case 16: instArr[arrCounter].dtAddr = (uint64_t)atoi(token); break;
                    case 18: instArr[arrCounter].condBrAddr = (uint64_t)atoi(token); break;
                    case 20: instArr[arrCounter].brAddr = (uint64_t)atoi(token); break;
                }
            }
            token = strtok(NULL, ", ");
            oddCount++;
        }
        arrCounter++;
    }
    fclose(instFile);
    *instCounter = arrCounter;
    return instArr;
}

int main()
{
    binary_instruction *instArray;
    int arrSize = 50;
    instArray = (binary_instruction*)calloc(arrSize,sizeof(binary_instruction));
    int instCount;
    binary_instruction *instArr = parseTextFile(instArray,arrSize,&instCount);

    machine_state m;
    m.pc = 0;
    m.registers = calloc(32, sizeof(uint64_t));
    m.stackSize = 512;
    m.stack = calloc(512, sizeof(uint8_t));
    m.memorySize = 4096;
    m.memory = calloc(4096, sizeof(uint8_t));
    m.registers[28] = 512; //init SP
    m.registers[29] = 512; //init FP

    emulate(instArr, instCount, &m);
    return 0;
}
