#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned h:1; //0b0
    unsigned imm10L:10;
    unsigned j2:1;
    unsigned bit12:1; //0b0
    unsigned j1:1;
    unsigned bits:2; //0b11
    unsigned imm10H:10;
    unsigned s:1;
	unsigned branch:5; //0b11110
} BLX_INST;

typedef struct {
    unsigned imm11:11;
    unsigned j2:1;
    unsigned bit12:1; //0b1
    unsigned j1:1;
    unsigned bits:2; //0b11
    unsigned imm10:10;
    unsigned s:1;
	unsigned branch:5; //0b11110
} BL_INST;

typedef struct {
    unsigned imm11:11;
    unsigned j2:1;
    unsigned bit12:1; //0b1
    unsigned j1:1;
    unsigned bits:2; //0b10
    unsigned imm10:10;
    unsigned s:1;
	unsigned branch:5; //0b11110
} B_W_INST;

typedef struct {
    unsigned align:2;
    unsigned imm10L:10;
    unsigned imm10H:10;
    unsigned i2:1; //I2 = NOT(J2 EOR S)
    unsigned i1:1; //I1 = NOT(J1 EOR S)
    unsigned s:1;
	unsigned HIGH_PAD:7;
} BLX_addr;

typedef struct {
    unsigned zero:1;
    unsigned imm11:11;
    unsigned imm10:10;
    unsigned i2:1; //I2 = NOT(J2 EOR S)
    unsigned i1:1; //I1 = NOT(J1 EOR S)
    unsigned s:1;
	unsigned HIGH_PAD:7;
} BL_addr;

typedef struct {
    unsigned zero:1;
    unsigned imm11:11;
    unsigned imm10:10;
    unsigned i2:1; //I2 = NOT(J2 EOR S)
    unsigned i1:1; //I1 = NOT(J1 EOR S)
    unsigned s:1;
	unsigned HIGH_PAD:7;
} B_W_addr;

void detour(void *src, void *dst) {
	unsigned int offset = (unsigned int)dst - (((unsigned int)src & 0xFFFFFFFE) + 4);
	B_W_addr addr;
	memcpy(&addr, &offset, sizeof(BL_addr));

	B_W_INST inst;
	inst.branch = 0b11110;
	inst.s = addr.s;
	inst.imm11 = addr.imm11;
	inst.bits = 0b10;
	inst.j1 = (~addr.i1)^addr.s;
	inst.bit12 = 0b1;
	inst.j2 = (~addr.i2)^addr.s;
	inst.imm10 = addr.imm10;

	unsigned int final_inst = 0;
	final_inst |= (*(unsigned int *)&inst & 0xFF0000) << 8;
	final_inst |= (*(unsigned int *)&inst & 0xFF000000) >> 8;
	final_inst |= (*(unsigned int *)&inst & 0xFF) << 8;
	final_inst |= (*(unsigned int *)&inst & 0xFF00) >> 8;

    printf("source: %X\n", (unsigned int)src);
    printf("dest: %X\n", (unsigned int)dst);
    printf("offset: %X\n", offset);
    printf("final inst: %X\n", (unsigned int)final_inst);

	for(int i = 0; i < 32; i++){
        if(*(unsigned int *)&inst & (0x80000000 >> i)){
            putc('1', stdout);
        } else {
            putc('0', stdout);
        }
        if(!((i + 1) % 4)) putc(' ', stdout);
	}
	putc('\n', stdout);
}

int main()
{
    detour((void *)0x4B97F6, (void *)0x6687B4);
    return 0;
}
