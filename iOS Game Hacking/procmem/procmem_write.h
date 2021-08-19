#include <stdlib.h>
#include <stdio.h>
#include <mach/mach.h>
#include <libkern/OSCacheControl.h>

#ifndef __BOOL_DEBUG
#define __BOOL_DEBUG
extern bool debug;
#endif

extern bool no_flush;

#ifndef __PROCMEM_RW_MODE
#define __PROCMEM_RW_MODE
typedef enum {
	_HEX_RAW,
	_HEX_INT,
	_SIGNED_INT,
	_UNSIGNED_INT,
	_FLOAT,
	_DOUBLE,
	_BOOLEAN,
	_ASCII,
	_RW_MODE_NULL /* Default */
} procmem_rw_mode;
#endif

void _procmem_write(mach_port_t task, vm_address_t address, void *buffer, size_t size);
void procmem_write_hex_raw(mach_port_t task, vm_address_t address, void *data);
void procmem_write_hex_int(mach_port_t task, vm_address_t address, void *data);
void procmem_write_signed_int(mach_port_t task, vm_address_t address, void *data);
void procmem_write_unsigned_int(mach_port_t task, vm_address_t address, void *data);
void procmem_write_float(mach_port_t task, vm_address_t address, void *data);
void procmem_write_double(mach_port_t task, vm_address_t address, void *data);
void procmem_write_boolean(mach_port_t task, vm_address_t address, void *data);
void procmem_write_ascii(mach_port_t task, vm_address_t address, void *data);

void procmem_write(mach_port_t task, vm_address_t address, void *data, procmem_rw_mode rw_mode) {
	switch(rw_mode) {
		case _HEX_RAW:
			procmem_write_hex_raw(task, address, data);
			break;
		case _HEX_INT:
			procmem_write_hex_int(task, address, data);
			break;
		case _SIGNED_INT:
			procmem_write_signed_int(task, address, data);
			break;
		case _UNSIGNED_INT:
			procmem_write_unsigned_int(task, address, data);
			break;
		case _FLOAT:
			procmem_write_float(task, address, data);
			break;
		case _DOUBLE:
			procmem_write_double(task, address, data);
			break;
		case _ASCII:
			procmem_write_ascii(task, address, data);
			break;
		default:
			printf("Invalid write mode.\n");
			break;
	}
}

void _procmem_write(mach_port_t task, vm_address_t address, void *buffer, size_t size) {
	kern_return_t res;
	vm_region_basic_info_data_t info;
	vm_size_t vmsize;
	vm_address_t region_address = address;
	mach_msg_type_number_t info_count = VM_REGION_BASIC_INFO_COUNT;
	memory_object_name_t object;
	
	res = vm_region(task, &region_address, &vmsize, VM_REGION_BASIC_INFO, (vm_region_info_t)&info, &info_count, &object);
	if (res != KERN_SUCCESS) {
		printf("procmem_write: vm_region: %s\n", mach_error_string(res));
		exit(0);
	}
	if (debug) printf("region protection: %c%c%c\n", (info.protection & VM_PROT_READ) ? 'r':'-', (info.protection & VM_PROT_WRITE) ? 'w':'-', (info.protection & VM_PROT_EXECUTE) ? 'x':'-'); // Debug output
	if (debug) printf("procmem_write: Address: 0x%X\nprocmem_write: Size: %lu\n", address, size); // Debug output
	
	res = vm_protect(task, address, size, NO, info.protection | VM_PROT_READ | VM_PROT_WRITE);
	if (debug) printf("procmem_write: vm_protect: preparing for writing\n"); // Debug output
	if (res != KERN_SUCCESS) {
		printf("procmem_write: vm_protect: %s\n", mach_error_string(res));
		exit(0);
	}
	if (debug) printf("new region protection: %c%c%c\n", (info.protection & VM_PROT_READ) ? 'r':'-', (info.protection & VM_PROT_WRITE) ? 'w':'-', (info.protection & VM_PROT_EXECUTE) ? 'x':'-'); // Debug output
	if (debug) printf("procmem_write: vm_write: writing...\n"); // Debug output
	res = vm_write(task, address, (vm_address_t)buffer, size);
	if (res != KERN_SUCCESS) {
		printf("procmem_write: vm_write: %s\n", mach_error_string(res));
		exit(0);
	}
	if (info.protection & VM_PROT_EXECUTE) {
		printf("Writing to executable region.\n");
		if (!no_flush) {
			if (debug) printf("procmem_write: sys_dcache_flush\n"); // Debug output
			sys_dcache_flush((void *)address, size);
			if (debug) printf("procmem_write: sys_icache_invalidate\n"); // Debug output
			sys_icache_invalidate((void *)address, size);
		}
	}
	if (debug) printf("procmem_write: vm_protect: reverting changes\n"); // Debug output
	res = vm_protect(task, address, size, NO, info.protection);
	if (res != KERN_SUCCESS) {
		printf("procmem_write: vm_protect: %s\n", mach_error_string(res));
		exit(0);
	}
	if (debug) printf("region protection: %c%c%c\n", (info.protection & VM_PROT_READ) ? 'r':'-', (info.protection & VM_PROT_WRITE) ? 'w':'-', (info.protection & VM_PROT_EXECUTE) ? 'x':'-'); // Debug output
	printf("Write successful.\n");
}

void procmem_write_hex_raw(mach_port_t task, vm_address_t address, void *data) {
	size_t size = strlen(data)/2;
	unsigned char substr[3] = {0, 0, 0};
	unsigned char *buffer = (unsigned char *)malloc(size);
	for(int i = 0; i < size; i++) {
		memcpy(substr, data+i*2, 2);
		sscanf((char *)substr, "%hhx", &buffer[i]);
	}
	
	/**** START DEBUG OUTPUT ****/
	if (debug) {
		printf("Write\nprocmem_write_hex_raw: Address: 0x%X\nprocmem_write_hex_raw: Size: %lu\nprocmem_write_hex_raw: To be written: ", address, size);
		for(int i = 0; i < size; i++) {
			printf("%02hhX ", buffer[i]);
			if (((i + 1) % 4) == 0) {
				printf("  ");
			}
			if (((i + 1) % 16) == 0) {
				printf("\nprocmem_write_hex_raw: To be written: ");
			}
		}
		printf("\nprocmem_write_hex_raw: Buffer: %p\n", buffer);
	}
	/***** END DEBUG OUTPUT *****/
	
	_procmem_write(task, address, buffer, size);
	free(buffer);
}

void procmem_write_hex_int(mach_port_t task, vm_address_t address, void *data) {
	unsigned int actual_data = strtoul(data, NULL, 16);
	/**** START DEBUG OUTPUT ****/
	if (debug) printf("Write\nprocmem_write_hex_int: Address: 0x%X\nprocmem_write_hex_int: Size: %lu\nprocmem_write_hex_int: To be written: %X\n", address, sizeof(unsigned int), actual_data);
	/***** END DEBUG OUTPUT *****/
	_procmem_write(task, address, &actual_data, sizeof(unsigned int));
}

void procmem_write_signed_int(mach_port_t task, vm_address_t address, void *data) {
	int actual_data = atoi(data);
	if (debug) printf("Write\nprocmem_write_signed_int: Address: 0x%X\nprocmem_write_signed_int: Size: %lu\nprocmem_write_signed_int: To be written: %i\n", address, sizeof(int), actual_data);
	/***** END DEBUG OUTPUT *****/
	_procmem_write(task, address, &actual_data, sizeof(int));
}

void procmem_write_unsigned_int(mach_port_t task, vm_address_t address, void *data) {
	unsigned int actual_data = strtoul(data, NULL, 10);
	if (debug) printf("Write\nprocmem_write_unsigned_int: Address: 0x%X\nprocmem_write_unsigned_int: Size: %lu\nprocmem_write_unsigned_int: To be written: %u\n", address, sizeof(unsigned int), actual_data);
	/***** END DEBUG OUTPUT *****/
	_procmem_write(task, address, &actual_data, sizeof(unsigned int));
}

void procmem_write_float(mach_port_t task, vm_address_t address, void *data) {
	float actual_data = strtof(data, NULL);
	if (debug) printf("Write\nprocmem_write_float: Address: 0x%X\nprocmem_write_float: Size: %lu\nprocmem_write_float: To be written: %f\n", address, sizeof(float), actual_data);
	//sscanf(data, "%f", &actual_data);
	_procmem_write(task, address, &actual_data, sizeof(float));
}

void procmem_write_double(mach_port_t task, vm_address_t address, void *data) {
	double actual_data = strtod(data, NULL);
	if (debug) printf("Write\nprocmem_write_double: Address: 0x%X\nprocmem_write_double: Size: %lu\nprocmem_write_double: To be written: %lf\n", address, sizeof(double), actual_data);
	//sscanf(data, "%lf", &actual_data);
	_procmem_write(task, address, &actual_data, sizeof(double));
}

void procmem_write_ascii(mach_port_t task, vm_address_t address, void *data) {
	size_t size = strlen(data)+1;
	
	/**** START DEBUG OUTPUT ****/
	if (debug) {
		printf("Write\nprocmem_write_ascii: Address: 0x%X\nprocmem_write_ascii: Size: %lu\nprocmem_write_ascii: To be written: ", address, size);
		for(int i = 0; i < size; i++) {
			printf("%c ", ((char *)data)[i]);
			if (((i + 1) % 4) == 0) {
				printf("  ");
			}
			if (((i + 1) % 16) == 0) {
				printf("\nprocmem_write_ascii: To be written: ");
			}
		}
		printf("\nprocmem_write_ascii: Buffer: %p\n", data);
	}
	/***** END DEBUG OUTPUT *****/
	
	_procmem_write(task, address, data, size);
}