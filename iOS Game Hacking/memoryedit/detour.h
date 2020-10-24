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

bool detour(void *src, void *dst) {
	vm_region_basic_info_data_t info;
	mach_port_t port = mach_task_self();
	unsigned int offset = (unsigned int)dst - (((unsigned int)src & 0xFFFFFFFE) + 4);
	BL_addr addr;
	memcpy(&addr, &offset, sizeof(BL_addr));

	BL_INST inst;
	inst.branch = 0b11110;
	inst.s = addr.s;
	inst.imm11 = addr.imm11;
	inst.bits = 0b11;
	inst.j1 = (~addr.i1)^addr.s;
	inst.bit12 = 0b1;
	inst.j2 = (~addr.i2)^addr.s;
	inst.imm10 = addr.imm10;
	
	unsigned int final_inst = 0;
	final_inst |= (*(unsigned int *)&inst & 0xFFFF0000) >> 16;
	final_inst |= (*(unsigned int *)&inst & 0xFFFF) << 16;

	if(!getRegionInfo((vm_address_t)src, &info))
		return FALSE;
	
	vm_protect(port, (vm_address_t)src, sizeof(final_inst), NO, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_COPY);
	vm_write(port,(vm_address_t)src, (vm_address_t)&final_inst, sizeof(final_inst));
	vm_protect(port, (vm_address_t)src, sizeof(final_inst), NO, info.protection);

	return TRUE;
}

bool detour_w(void *src, void *dst) {
	vm_region_basic_info_data_t info;
	mach_port_t port = mach_task_self();
	unsigned int offset = (unsigned int)dst - (((unsigned int)src & 0xFFFFFFFE) + 4);
	B_W_addr addr;
	memcpy(&addr, &offset, sizeof(B_W_addr));

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
	final_inst |= (*(unsigned int *)&inst & 0xFFFF0000) >> 16;
	final_inst |= (*(unsigned int *)&inst & 0xFFFF) << 16;

	if(!getRegionInfo((vm_address_t)src, &info))
		return FALSE;
	
	vm_protect(port, (vm_address_t)src, sizeof(final_inst), NO, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_COPY);
	vm_write(port,(vm_address_t)src, (vm_address_t)&final_inst, sizeof(final_inst));
	sys_dcache_flush(src, sizeof(final_inst));
	sys_icache_invalidate((src, sizeof(final_inst));
	vm_protect(port, (vm_address_t)src, sizeof(final_inst), NO, info.protection);

	return TRUE;
}