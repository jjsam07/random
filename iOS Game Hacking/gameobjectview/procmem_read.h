#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <mach/mach.h>

#ifndef __BOOL_DEBUG
#define __BOOL_DEBUG
extern bool debug;
#endif

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
	_RW_MODE_NULL
} procmem_rw_mode;
#endif

void procmem_read_hex_raw(vm_address_t address, void *buffer, size_t size);
void procmem_read_hex_int(vm_address_t address, void *buffer, size_t size);
void procmem_read_signed_int(vm_address_t address, void *buffer, size_t size);
void procmem_read_unsigned_int(vm_address_t address, void *buffer, size_t size);
void procmem_read_float(vm_address_t address, void *buffer, size_t size);
void procmem_read_double(vm_address_t address, void *buffer, size_t size);
void procmem_read_boolean(vm_address_t address, void *buffer, size_t size);

void procmem_read(vm_address_t address, void *buffer, size_t size, procmem_rw_mode mode) {
	switch(mode) {
		case _HEX_RAW:
			procmem_read_hex_raw(address, buffer, size);
			break;
		case _HEX_INT:
			procmem_read_hex_int(address, buffer, size);
			break;
		case _SIGNED_INT:
			procmem_read_signed_int(address, buffer, size);
			break;
		case _UNSIGNED_INT:
			procmem_read_unsigned_int(address, buffer, size);
			break;
		case _FLOAT:
			procmem_read_float(address, buffer, size);
			break;
		case _DOUBLE:
			procmem_read_double(address, buffer, size);
			break;
		case _BOOLEAN:
			procmem_read_boolean(address, buffer, size);
			break;
		default:
			printf("Invalid read mode,\n");
			exit(0);
			break;
	}
}

void procmem_read_hex_raw(vm_address_t address, void *buffer, size_t size) {
	printw("0x%X: ", address);
	for (int i = 0; i < size; i++) {	
		printw("%02hhX ", ((unsigned char *)buffer)[i]); // ncurses
		
		if (((i + 1) % 4) == 0) {
			addstr("  "); // ncurses
		}
		if (((i + 1) % 16) == 0) {
			printw("\n0x%X: ", address+i+1); // ncurses
		}
	}
}

void procmem_read_hex_int(vm_address_t address, void *buffer, size_t size) {
	printw("0x%X: ", address);
	for (int i = 0; i < (size/sizeof(unsigned int)); i++) {	
		printw("%8X  ", ((unsigned int *)buffer)[i]); // ncurses
		
		if (((i + 1) % 4) == 0) {
			printw("\n0x%X: ", address+((i+1)*sizeof(unsigned int))); // ncurses
		}
	}
}

void procmem_read_signed_int(vm_address_t address, void *buffer, size_t size) {
	printw("0x%X: ", address);
	for (int i = 0; i < (size/sizeof(int)); i++) {	
		printw("%11i  ", ((int *)buffer)[i]); // ncurses
		
		if (((i + 1) % 4) == 0) {
			printw("\n0x%X: ", address+((i+1)*sizeof(int))); // ncurses
		}
	}
}

void procmem_read_unsigned_int(vm_address_t address, void *buffer, size_t size) {
	printw("0x%X: ", address);
	for (int i = 0; i < (size/sizeof(unsigned int)); i++) {	
		printw("%10u  ", ((unsigned int *)buffer)[i]); // ncurses
		
		if (((i + 1) % 4) == 0) {
			printw("\n0x%X: ", address+((i+1)*sizeof(unsigned int))); // ncurses
		}
	}
}

void procmem_read_float(vm_address_t address, void *buffer, size_t size) {
	for (int i = 0; i < (size/sizeof(float)); i++) {	
		printw("0x%X: %f\n", address+(i*sizeof(float)), ((float *)buffer)[i]); // ncurses
	}
}

void procmem_read_double(vm_address_t address, void *buffer, size_t size) {
	for (int i = 0; i < (size/sizeof(double)); i++) {	
		printw("0x%X: %lf\n", address+(i*sizeof(double)), ((double *)buffer)[i]); // ncurses
	}
}

void procmem_read_boolean(vm_address_t address, void *buffer, size_t size) {
	printw("0x%X: ", address);
	for (int i = 0; i < (size/sizeof(bool)); i++) {	
		printw("%5s  ", ((bool *)buffer)[i] ? "TRUE":"FALSE"); // ncurses
		
		if (((i + 1) % 4) == 0) {
			printw("\n0x%X: ", address+((i+1)*sizeof(bool))); // ncurses
		}
	}
}