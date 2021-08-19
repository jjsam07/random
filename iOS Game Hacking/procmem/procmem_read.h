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
	_ASCII,
	_RW_MODE_NULL /* Default */
} procmem_rw_mode;
#endif

void procmem_read_hex_raw(mach_port_t task, vm_address_t address, void *buffer, size_t size);
void procmem_read_hex_int(mach_port_t task, vm_address_t address, void *buffer, size_t size);
void procmem_read_signed_int(mach_port_t task, vm_address_t address, void *buffer, size_t size);
void procmem_read_unsigned_int(mach_port_t task, vm_address_t address, void *buffer, size_t size);
void procmem_read_float(mach_port_t task, vm_address_t address, void *buffer, size_t size);
void procmem_read_double(mach_port_t task, vm_address_t address, void *buffer, size_t size);
void procmem_read_boolean(mach_port_t task, vm_address_t address, void *buffer, size_t size);
void procmem_read_ascii(mach_port_t task, vm_address_t address, void *buffer, size_t size);

void procmem_read(mach_port_t task, vm_address_t address, size_t size, procmem_rw_mode mode) {
	void *buffer = NULL;
	/**** START DEBUG OUTPUT ****/
	if (debug) printf("Read\nprocmem_read: Address: 0x%X\nprocmem_read: Size: %lu\nprocmem_read: Buffer: %p\n", address, size, buffer);
	/***** END DEBUG OUTPUT *****/
	switch(mode) {
		case _HEX_RAW:
			buffer = malloc(size);
			procmem_read_hex_raw(task, address, buffer, size);
			free(buffer);
			break;
		case _HEX_INT:
			size = size*sizeof(unsigned int);
			buffer = malloc(size);
			procmem_read_hex_int(task, address, buffer, size);
			free(buffer);
			break;
		case _SIGNED_INT:
			size = size*sizeof(int);
			buffer = malloc(size);
			procmem_read_signed_int(task, address, buffer, size);
			free(buffer);
			break;
		case _UNSIGNED_INT:
			size = size*sizeof(unsigned int);
			buffer = malloc(size);
			procmem_read_unsigned_int(task, address, buffer, size);
			free(buffer);
			break;
		case _FLOAT:
			size = size*sizeof(float);
			buffer = malloc(size);
			procmem_read_float(task, address, buffer, size);
			free(buffer);
			break;
		case _DOUBLE:
			size = size*sizeof(double);
			buffer = malloc(size);
			procmem_read_double(task, address, buffer, size);
			free(buffer);
			break;
		case _BOOLEAN:
			size = size*sizeof(bool);
			buffer = malloc(size);
			procmem_read_boolean(task, address, buffer, size);
			free(buffer);
			break;
		case _ASCII:
			buffer = malloc(size);
			procmem_read_ascii(task, address, buffer, size);
			free(buffer);
			break;
		default:
			printf("Invalid read mode,\n");
			break;
	}
	free(buffer);
}

void procmem_read_hex_raw(mach_port_t task, vm_address_t address, void *buffer, size_t size) {
	/**** START DEBUG OUTPUT ****/
	if (debug) printf("Read\nprocmem_read_hex_raw: Address: 0x%X\nprocmem_read_hex_raw: Size: %lu\nprocmem_read_hex_raw: Buffer: %p\n", address, size, buffer);
	/***** END DEBUG OUTPUT *****/
	kern_return_t res;
	initscr();
	cbreak();
	noecho();
	while(1) {
		res = vm_read_overwrite(task, address, size, (vm_address_t)buffer, (vm_size_t *)&size);
		/**** START DEBUG OUTPUT ****/
		if (debug) printf("procmem_read_hex_raw: vm_read_overwrite\n");
		/***** END DEBUG OUTPUT *****/
		if (res != KERN_SUCCESS) {
			endwin();
			printf("vm_read_overwrite: Read error: %s\n", mach_error_string(res));
			exit(0);
		}
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
		refresh();
		usleep(250);
		clear();
	}
	endwin();
}

void procmem_read_hex_int(mach_port_t task, vm_address_t address, void *buffer, size_t size) {
	/**** START DEBUG OUTPUT ****/
	if (debug) printf("Read\nprocmem_read_hex_int: Address: 0x%X\nprocmem_read_hex_int: Size: %lu\nprocmem_read_hex_int: Buffer: %p\n", address, size, buffer);
	/***** END DEBUG OUTPUT *****/
	kern_return_t res;
	initscr();
	cbreak();
	noecho();
	while(1) {
		res = vm_read_overwrite(task, address, size, (vm_address_t)buffer, (vm_size_t *)&size);
		/**** START DEBUG OUTPUT ****/
		if (debug) printf("procmem_read_hex_int: vm_read_overwrite\n");
		/***** END DEBUG OUTPUT *****/
		if (res != KERN_SUCCESS) {
			endwin();
			printf("vm_read_overwrite: Read error: %s\n", mach_error_string(res));
			exit(0);
		}
		printw("0x%X: ", address);
		for (int i = 0; i < (size/sizeof(unsigned int)); i++) {	
			printw("%8X  ", ((unsigned int *)buffer)[i]); // ncurses
			
			if (((i + 1) % 4) == 0) {
				printw("\n0x%X: ", address+((i+1)*sizeof(unsigned int))); // ncurses
			}
		}
		refresh();
		usleep(250);
		clear();
	}
	endwin();
}

void procmem_read_signed_int(mach_port_t task, vm_address_t address, void *buffer, size_t size) {
	/**** START DEBUG OUTPUT ****/
	if (debug) printf("Read\nprocmem_read_signed_int: Address: 0x%X\nprocmem_read_signed_int: Size: %lu\nprocmem_read_signed_int: Buffer: %p\n", address, size, buffer);
	/***** END DEBUG OUTPUT *****/
	kern_return_t res;
	initscr();
	cbreak();
	noecho();
	while(1) {
		res = vm_read_overwrite(task, address, size, (vm_address_t)buffer, (vm_size_t *)&size);
		/**** START DEBUG OUTPUT ****/
		if (debug) printf("procmem_read_signed_int: vm_read_overwrite\n");
		/***** END DEBUG OUTPUT *****/
		if (res != KERN_SUCCESS) {
			endwin();
			printf("vm_read_overwrite: Read error: %s\n", mach_error_string(res));
			exit(0);
		}
		printw("0x%X: ", address);
		for (int i = 0; i < (size/sizeof(int)); i++) {	
			printw("%11i  ", ((int *)buffer)[i]); // ncurses
			
			if (((i + 1) % 4) == 0) {
				printw("\n0x%X: ", address+((i+1)*sizeof(int))); // ncurses
			}
		}
		refresh();
		usleep(250);
		clear();
	}
	endwin();
}

void procmem_read_unsigned_int(mach_port_t task, vm_address_t address, void *buffer, size_t size) {
	/**** START DEBUG OUTPUT ****/
	if (debug) printf("Read\nprocmem_read_signed_int: Address: 0x%X\nprocmem_read_signed_int: Size: %lu\nprocmem_read_signed_int: Buffer: %p\n", address, size, buffer);
	/***** END DEBUG OUTPUT *****/
	kern_return_t res;
	initscr();
	cbreak();
	noecho();
	while(1) {
		res = vm_read_overwrite(task, address, size, (vm_address_t)buffer, (vm_size_t *)&size);
		/**** START DEBUG OUTPUT ****/
		if (debug) printf("procmem_read_unsigned_int: vm_read_overwrite\n");
		/***** END DEBUG OUTPUT *****/
		if (res != KERN_SUCCESS) {
			endwin();
			printf("vm_read_overwrite: Read error: %s\n", mach_error_string(res));
			exit(0);
		}
		printw("0x%X: ", address);
		for (int i = 0; i < (size/sizeof(unsigned int)); i++) {	
			printw("%10u  ", ((unsigned int *)buffer)[i]); // ncurses
			
			if (((i + 1) % 4) == 0) {
				printw("\n0x%X: ", address+((i+1)*sizeof(unsigned int))); // ncurses
			}
		}
		refresh();
		usleep(250);
		clear();
	}
	endwin();
}

void procmem_read_float(mach_port_t task, vm_address_t address, void *buffer, size_t size) {
	/**** START DEBUG OUTPUT ****/
	if (debug) printf("Read\nprocmem_read_signed_int: Address: 0x%X\nprocmem_read_signed_int: Size: %lu\nprocmem_read_signed_int: Buffer: %p\n", address, size, buffer);
	/***** END DEBUG OUTPUT *****/
	kern_return_t res;
	initscr();
	cbreak();
	noecho();
	while(1) {
		res = vm_read_overwrite(task, address, size, (vm_address_t)buffer, (vm_size_t *)&size);
		/**** START DEBUG OUTPUT ****/
		if (debug) printf("procmem_read_float: vm_read_overwrite\n");
		/***** END DEBUG OUTPUT *****/
		if (res != KERN_SUCCESS) {
			endwin();
			printf("vm_read_overwrite: Read error: %s\n", mach_error_string(res));
			exit(0);
		}
		for (int i = 0; i < (size/sizeof(float)); i++) {	
			printw("0x%X: %f\n", address+(i*sizeof(float)), ((float *)buffer)[i]); // ncurses
		}
		refresh();
		usleep(250);
		clear();
	}
	endwin();
}

void procmem_read_double(mach_port_t task, vm_address_t address, void *buffer, size_t size) {
	/**** START DEBUG OUTPUT ****/
	if (debug) printf("Read\nprocmem_read_signed_int: Address: 0x%X\nprocmem_read_signed_int: Size: %lu\nprocmem_read_signed_int: Buffer: %p\n", address, size, buffer);
	/***** END DEBUG OUTPUT *****/
	kern_return_t res;
	initscr();
	cbreak();
	noecho();
	while(1) {
		res = vm_read_overwrite(task, address, size, (vm_address_t)buffer, (vm_size_t *)&size);
		/**** START DEBUG OUTPUT ****/
		if (debug) printf("procmem_read_double: vm_read_overwrite\n");
		/***** END DEBUG OUTPUT *****/
		if (res != KERN_SUCCESS) {
			endwin();
			printf("vm_read_overwrite: Read error: %s\n", mach_error_string(res));
			exit(0);
		}
		for (int i = 0; i < (size/sizeof(double)); i++) {	
			printw("0x%X: %lf\n", address+(i*sizeof(double)), ((double *)buffer)[i]); // ncurses
		}
		refresh();
		usleep(250);
		clear();
	}
	endwin();
}

void procmem_read_boolean(mach_port_t task, vm_address_t address, void *buffer, size_t size) {
	/**** START DEBUG OUTPUT ****/
	if (debug) printf("Read\nprocmem_read_signed_int: Address: 0x%X\nprocmem_read_signed_int: Size: %lu\nprocmem_read_signed_int: Buffer: %p\n", address, size, buffer);
	/***** END DEBUG OUTPUT *****/
	kern_return_t res;
	initscr();
	cbreak();
	noecho();
	while(1) {
		res = vm_read_overwrite(task, address, size, (vm_address_t)buffer, (vm_size_t *)&size);
		/**** START DEBUG OUTPUT ****/
		if (debug) printf("procmem_read_boolean: vm_read_overwrite\n");
		/***** END DEBUG OUTPUT *****/
		if (res != KERN_SUCCESS) {
			endwin();
			printf("vm_read_overwrite: Read error: %s\n", mach_error_string(res));
			exit(0);
		}
		printw("0x%X: ", address);
		for (int i = 0; i < (size/sizeof(bool)); i++) {	
			printw("%5s  ", ((bool *)buffer)[i] ? "TRUE":"FALSE"); // ncurses
			
			if (((i + 1) % 4) == 0) {
				printw("\n0x%X: ", address+((i+1)*sizeof(bool))); // ncurses
			}
		}
		refresh();
		usleep(250);
		clear();
	}
	endwin();
}

void procmem_read_ascii(mach_port_t task, vm_address_t address, void *buffer, size_t size) {
	/**** START DEBUG OUTPUT ****/
	if (debug) printf("Read\nprocmem_read_ascii: Address: 0x%X\nprocmem_read_ascii: Size: %lu\nprocmem_read_ascii: Buffer: %p\n", address, size, buffer);
	/***** END DEBUG OUTPUT *****/
	kern_return_t res;
	initscr();
	cbreak();
	noecho();
	while(1) {
		res = vm_read_overwrite(task, address, size, (vm_address_t)buffer, (vm_size_t *)&size);
		/**** START DEBUG OUTPUT ****/
		if (debug) printf("procmem_read_ascii: vm_read_overwrite\n");
		/***** END DEBUG OUTPUT *****/
		if (res != KERN_SUCCESS) {
			endwin();
			printf("vm_read_overwrite: Read error: %s\n", mach_error_string(res));
			exit(0);
		}
		printw("0x%X: ", address);
		for (int i = 0; i < size; i++) {	
			printw("%c ", ((unsigned char *)buffer)[i]); // ncurses
			
			if (((i + 1) % 4) == 0) {
				addstr("  "); // ncurses
			}
			if (((i + 1) % 16) == 0) {
				printw("\n0x%X: ", address+i+1); // ncurses
			}
		}
		refresh();
		usleep(250);
		clear();
	}
	endwin();
}