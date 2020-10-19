#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#import <mach/mach.h>
#import <mach/mach_traps.h>

FILE *f;
mach_port_t port;

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
    unsigned zero:1;
    unsigned imm11:11;
    unsigned imm10:10;
    unsigned i2:1; //I2 = NOT(J2 EOR S)
    unsigned i1:1; //I1 = NOT(J1 EOR S)
    unsigned s:1;
	unsigned HIGH_PAD:7;
} B_W_addr;

bool getRegionInfo(vm_address_t address, vm_region_basic_info_data_t *info) {

	vm_size_t vmsize;
	mach_msg_type_number_t info_count = VM_REGION_BASIC_INFO_COUNT;
	memory_object_name_t object;

	kern_return_t retval = vm_region(mach_task_self(), &address, &vmsize, VM_REGION_BASIC_INFO, (vm_region_info_t)info, &info_count, &object);
	if (retval != KERN_SUCCESS)
		return FALSE;

	return TRUE;

}

bool detour(void *src, void *dst) {
	vm_region_basic_info_data_t info;
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

	//final_inst = 0xFFFFFFFF;

	if(!getRegionInfo((vm_address_t)src, &info))
		return FALSE;
	
	vm_protect(port, (vm_address_t)src, sizeof(final_inst), NO, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_COPY);
	vm_write(port,(vm_address_t)src, (vm_address_t)&final_inst, sizeof(final_inst));
	vm_protect(port, (vm_address_t)src, sizeof(final_inst), NO, info.protection);

	return TRUE;
}

void *hook_calloc(size_t size1, size_t size2) {
	void *ret = calloc(size1, size2);
	fprintf(f, "C: %" PRIXPTR ":%lX:%lX\n", (uintptr_t)ret, size1, size2);
	fflush(f);
	return ret;
}

void *hook_malloc(size_t size) {
	void *ret = malloc(size);
	fprintf(f, "M: %" PRIXPTR ":%lX\n", (uintptr_t)ret, size);
	fflush(f);
	return ret;
}

void *hook_realloc(void *ptr, size_t size){
	void *ret = realloc(ptr, size);
	fprintf(f, "R: %" PRIXPTR ":%lX\n", (uintptr_t)ret, size);
	fflush(f);
	return ret;
}

%ctor {
	f = fopen("/var/mobile/detour.log", "w+");
	port = mach_task_self();
	//detour((void *)0x3116ca, (void *)hook_malloc); //j_mallocref
	detour((void *)0x4b97f6, (void *)hook_malloc); //j_mallocref
	//good
	/*detour((void *)0x618752, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x61875c, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x620c2e, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x621dc0, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x6228d6, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x62293e, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x626a12, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x626b28, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x629bce, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x629c1a, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x629fc6, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x62a010, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x62ef94, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x62f130, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x62f78c, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x62f796, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x62f7bc, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x62f7c8, (void *)hook_malloc); //j_j_mallocref1*/
	//end good
	//not good
	/*detour((void *)0x62f7d4, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x631572, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x6328e0, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x632d86, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x632eba, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x633034, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x63312a, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x63331c, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x634e1a, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x634e24, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x635366, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x635370, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x63539a, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x6353a6, (void *)hook_malloc); //j_j_mallocref1
	detour((void *)0x6353b2, (void *)hook_malloc); //j_j_mallocref1*/
	//end not good
	//good
	/*detour((void *)0x620b22, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x620b54, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x620fd6, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62106c, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x621142, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x621156, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62116c, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6212f8, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x621364, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62149e, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x621506, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x621736, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6217b0, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62196e, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6222ce, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6224ba, (void *)hook_malloc); //j_j_mallocref2*/
	//end good
	//good
	/*detour((void *)0x622564, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62288c, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6229c0, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x622d2a, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x622d44, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62304a, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x623b06, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62411e, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x624166, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6241a2, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6241ea, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x624226, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62427e, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6242e2, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62432e, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62437e, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6243ca, (void *)hook_malloc); //j_j_mallocref2*/
	//end good
	//good
	/*detour((void *)0x62441a, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x624466, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6244b4, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x624864, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6248d2, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6258f8, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x625ef8, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x626082, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6260de, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62616e, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6262bc, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x626a6c, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x626b84, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x626bac, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x627678, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6278a2, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6279c0, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x627b5c, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x627f32, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x627fa0, (void *)hook_malloc); //j_j_mallocref2*/
	//end good
	//good
	/*detour((void *)0x628348, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6298e8, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x629ae2, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x629b24, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x629b7c, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x629e72, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x629f12, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x629f6c, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62a350, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62a420, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62a46c, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62a5a0, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62a6da, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62a75a, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62a846, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62a8ae, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62a94e, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62a9e6, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62aaf6, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62ac04, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62ade4, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62ae80, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62aecc, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62af24, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62af4c, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62afb4, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62aff0, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62b10e, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62b178, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62b1fc, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62b664, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62b6fe, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62b77a, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62b8b2, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62b95a, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62b9aa, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62ba18, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62bf54, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62c06a, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62c24c, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62c422, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62c9cc, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62cb14, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62d000, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62d0fa, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62d1ae, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62d262, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62d916, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62d992, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62e03a, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62e1b8, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62e4e6, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62e62e, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62e6c6, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62e89e, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62e92e, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62e98a, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62ea6c, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62ead8, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62eb78, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62ebf4, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62efa8, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62f06c, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62f2fc, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62f454, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x62f5ac, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x630d5e, (void *)hook_malloc); //j_j_mallocref2*/
	//end good
	//not good
	/*detour((void *)0x632364, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x632458, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6325c8, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x632788, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x63290a, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x632992, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x632a00, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x632b16, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x632b48, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x632b8e, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x632ba4, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x632bb8, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x632bf8, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x632c0e, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x632c26, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x632c66, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x632c7c, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x632e14, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x633808, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6338dc, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x63399a, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6339cc, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x633ad2, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x633b02, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x633c48, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x633d14, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x633d4c, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x633e54, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x633e8a, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x633f8c, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x634170, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x63427c, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x63493a, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x6349b6, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x634a74, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x634bae, (void *)hook_malloc); //j_j_mallocref2
	detour((void *)0x634ca0, (void *)hook_malloc); //j_j_mallocref2*/
	//end not good
	/*detour((void *)0x2f8490, (void *)hook_realloc); //reallocref
	detour((void *)0x381550, (void *)hook_realloc); //reallocref
	detour((void *)0x381564, (void *)hook_realloc); //reallocref
	detour((void *)0x381574, (void *)hook_realloc); //reallocref
	detour((void *)0x60e642, (void *)hook_realloc); //reallocref
	detour((void *)0x60e656, (void *)hook_realloc); //reallocref
	detour((void *)0x60ec02, (void *)hook_realloc); //reallocref
	detour((void *)0x2f84e2, (void *)hook_realloc); //j_reallocref
	detour((void *)0x3116e8, (void *)hook_realloc); //j_reallocref
	detour((void *)0x152f44, (void *)hook_realloc); //j_j_reallocref
	detour((void *)0x152fd8, (void *)hook_realloc); //j_j_reallocref
	detour((void *)0x37f776, (void *)hook_realloc); //reallocfref */
}

__attribute__((destructor)) void lib_unload() {
fclose(f);
}