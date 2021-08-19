#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <mach/mach.h>
#include <libkern/OSCacheControl.h>
#include <sys/sysctl.h>
#include "procmem_read.h"
#include "procmem_write.h"

typedef enum {
	_READ,
	_WRITE,
	_LOCK,
	_MODE_NULL /* Default */
} procmem_mode;

bool debug = false;
bool no_flush = false;

void procmem_lock(mach_port_t task, vm_address_t address, size_t size) {
	kern_return_t res;
	unsigned char buffer[size];
	
	res = vm_read_overwrite(task, address, size, (vm_address_t)buffer, (vm_size_t *)&size);
	if (res != KERN_SUCCESS) {
		printf("vm_read_overwrite: Read error: %s\n", mach_error_string(res));
		exit(0);
	}
	while(1) {
		vm_write(task, address, (vm_address_t)buffer, size);
		if (res != KERN_SUCCESS) {
			printf("vm_write: %s\n", mach_error_string(res));
			exit(0);
		}
		//sys_dcache_flush((void *)address, size);
		//sys_icache_invalidate((void *)address, size);
	}
}

void print_usage(char *program_name) {
	puts("Usage:");
	printf("  %s -r [-p PROCESS] [-a ADDRESS] [-b|-xi|-si|-ui|-f|-lf|-bool|-ascii SIZE]\n", program_name);
	printf("  %s -w [-p PROCESS] [-a ADDRESS] [-b|-xi|-si|-ui|-f|-lf|-ascii DATA]\n", program_name);
	printf("  %s -l [-p PROCESS] [-a ADDRESS] [-s SIZE]\n", program_name);
	puts("\t-p\tprocess name or process ID");
	puts("\t-a\taddress in hexadecimal");
	puts("\t-s\tsize in bytes\n");
	puts("\tmodes:\n");
	puts("\t-r\tread SIZE bytes or elements at ADDRESS");
	puts("\t-w\twrite DATA at ADDRESS");
	puts("\t-l\tlock SIZE bytes at ADDRESS\n");
	puts("\t\t'lock' mode reads the value and continuously");
	puts("\t\twrites the same value at the given address.\n");
	puts("\tread/write options:\n");
	puts("\t-b\tread/write data as raw hex bytes");
	puts("\t-xi\tread/write data as hexadecimal");
	puts("\t-si\tread/write data as signed integer");
	puts("\t-ui\tread/write data as unsigned integer");
	puts("\t-f\tread/write data as floating-point");
	puts("\t-lf\tread/write data as double-precision floating-point");
	puts("\t-bool\tread data as boolean (TRUE/FALSE)");
	puts("\t-ascii\tread/write data as ASCII");
}

pid_t get_pid_by_name(char *process_name) {
	size_t sysctl_size;
	struct kinfo_proc *process_list = NULL;
	int sysctl_status;
	pid_t pid;

	int sysctl_mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
	
	if (debug) printf("struct kinfo_proc size: %lu\nMAXCOMLEN: %d\n", sizeof(struct kinfo_proc), MAXCOMLEN);
	
	sysctl_status = sysctl(sysctl_mib, 4, NULL, &sysctl_size, NULL, 0);
	if (debug) printf("sysctl status: %d\ndata size: %lu\n", sysctl_status, sysctl_size);
	process_list = malloc(sysctl_size);
	sysctl_status = sysctl(sysctl_mib, 4, process_list, &sysctl_size, NULL, 0);
	if (debug) printf("sysctl status: %d\ndata size: %lu\n", sysctl_status, sysctl_size);
	
	for(int i = 0; i < sysctl_size/sizeof(struct kinfo_proc); i++) {
		if (debug) printf("%s: %d\n", process_list[i].kp_proc.p_comm, process_list[i].kp_proc.p_pid);
		if (strncmp(process_list[i].kp_proc.p_comm, process_name, MAXCOMLEN) == 0) {
			pid = process_list[i].kp_proc.p_pid;
			free(process_list);
			return pid;
		}
	}
	
	free(process_list);
	
	return 0;
}

int main(int argc, char **argv, char **envp) {
	pid_t pid;
	vm_address_t address;
	size_t read_size = 0;
	size_t lock_size = 0;
	mach_port_t task;
	kern_return_t res;
	void *data = NULL;
	unsigned char *buf = NULL;
	int size_arg_index = 0;
	procmem_mode mode = _MODE_NULL;
	procmem_rw_mode rw_mode = _RW_MODE_NULL;
	
	/*
	Usage:
	  %s -r [-p PROCESS] [-a ADDRESS] [-b|-xi|-si|-ui|-f|-lf|-bool|-ascii SIZE]
	  %s -w [-p PROCESS] [-a ADDRESS] [-b|-xi|-si|-ui|-f|-lf|-ascii DATA]
	  %s -l [-p PROCESS] [-a ADDRESS] [-s SIZE]
	
		-p		process name or process ID
		-a		address in hexadecimal
		-s		size in bytes
		
		modes:
		
		-r		read SIZE bytes or elements at ADDRESS
		-w		write DATA at ADDRESS
		-l		lock SIZE bytes at ADDRESS
				
			'lock' mode reads the value and continuously
			writes the same value at the given address.
		
		read/write options:
		
		-b		read/write data as raw hex bytes
		-xi		read/write data as hexadecimal
		-si		read/write data as signed integer
		-ui		read/write data as unsigned integer
		-f		read/write data as floating-point
		-lf		read/write data as double-precision floating-point
		-bool	read data as boolean (TRUE/FALSE)
		-ascii	read/write data as ASCII
	*/
	
	if (argc < 5) {
		print_usage(argv[0]);
		return 0;
	}
	
	for(int i = 1; i < argc; i++){
		if (strcmp(argv[i], "-r") == 0) {
			if (mode == _MODE_NULL) {
				mode = _READ;
			}
			continue;
		}
		else if (strcmp(argv[i], "-w") == 0) {
			if (mode == _MODE_NULL) {
				mode = _WRITE;
			}
			continue;
		}
		else if (strcmp(argv[i], "-l") == 0) {
			if (mode == _MODE_NULL) {
				mode = _LOCK;
			}
			continue;
		}
		else if (strcmp(argv[i], "-p") == 0) {
			pid = get_pid_by_name(argv[i+1]);
			//pid = strtoul(argv[i+1], NULL, 10);
			i++;
			continue;
		}
		else if (strcmp(argv[i], "-a") == 0) {
			address = strtoul(argv[i+1], NULL, 16);
			i++;
			continue;
		}
		else if (strcmp(argv[i], "-debug") == 0) {
			debug = true;
			continue;
		}
		else if (strcmp(argv[i], "-no-flush") == 0) {
			no_flush = true;
			continue;
		}
		else if (strcmp(argv[i], "-s") == 0) {
			if (lock_size == 0) {
				lock_size = strtoul(argv[i+1], NULL, 10);
				if (lock_size == 0) {
					printf("%s %s\n", argv[i], argv[i+1]);
					printf("Invalid size.\n");
					return 0;
				}
			}
			i++;
			continue;
		}
		else if (strcmp(argv[i], "-b") == 0) {
			if (rw_mode == _RW_MODE_NULL) {
				rw_mode = _HEX_RAW;
			}
			if (data == NULL) {
				if (i+1 < argc) {
					data = argv[i+1];
				}
			}
			if (read_size == 0) {
				read_size = strtoul(argv[i+1], NULL, 10);
			}
			i++;
			continue;
		}
		else if (strcmp(argv[i], "-xi") == 0) {
			if (rw_mode == _RW_MODE_NULL) {
				rw_mode = _HEX_INT;
			}
			if (data == NULL) {
				if (i+1 < argc) {
					data = argv[i+1];
				}
			}
			if (read_size == 0){
				read_size = strtoul(argv[i+1], NULL, 10);
			}
			i++;
			continue;
		}
		else if (strcmp(argv[i], "-si") == 0) {
			if (rw_mode == _RW_MODE_NULL) {
				rw_mode = _SIGNED_INT;
			}
			if (data == NULL) {
				if (i+1 < argc) {
					data = argv[i+1];
				}
			}
			if (read_size == 0) {
				read_size = strtoul(argv[i+1], NULL, 10);
			}
			i++;
			continue;
		}
		else if (strcmp(argv[i], "-ui") == 0) {
			if (rw_mode == _RW_MODE_NULL) {
				rw_mode = _UNSIGNED_INT;
			}
			if (data == NULL) {
				if (i+1 < argc) {
					data = argv[i+1];
				}
			}
			if (read_size == 0) {
				read_size = strtoul(argv[i+1], NULL, 10);
			}
			i++;
			continue;
		}
		else if (strcmp(argv[i], "-f") == 0) {
			if (rw_mode == _RW_MODE_NULL) {
				rw_mode = _FLOAT;
			}
			if (data == NULL) {
				if (i+1 < argc) {
					data = argv[i+1];
				}
			}
			if (read_size == 0) {
				read_size = strtoul(argv[i+1], NULL, 10);
			}
			i++;
			continue;
		}
		else if (strcmp(argv[i], "-lf") == 0) {
			if (rw_mode == _RW_MODE_NULL) {
				rw_mode = _DOUBLE;
			}
			if (data == NULL) {
				if (i+1 < argc) {
					data = argv[i+1];
				}
			}
			if (read_size == 0) {
				read_size = strtoul(argv[i+1], NULL, 10);
			}
			i++;
			continue;
		}
		else if (strcmp(argv[i], "-bool") == 0) {
			if (rw_mode == _RW_MODE_NULL) {
				rw_mode = _BOOLEAN;
			}
			if (read_size == 0) {
				read_size = strtoul(argv[i+1], NULL, 10);
			}
			i++;
			continue;
		}
		else if (strcmp(argv[i], "-ascii") == 0) {
			if (rw_mode == _RW_MODE_NULL) {
				rw_mode = _ASCII;
			}
			if (data == NULL) {
				if (i+1 < argc) {
					data = argv[i+1];
				}
			}
			if (read_size == 0) {
				read_size = strtoul(argv[i+1], NULL, 10);
			}
			i++;
			continue;
		}
	}
	
	res = task_for_pid(mach_task_self(), pid, &task);
	if (res != KERN_SUCCESS) {
		printf("task_for_pid: %d: %s\n", pid, mach_error_string(res));
		return 0;
	}
	
	switch(mode) {
		case _READ:
			if (read_size == 0) {
				printf("\'%s %s\'\n", argv[size_arg_index], argv[size_arg_index+1]);
				printf("Invalid size.\n");
				puts("Specify size with [-b|-xi|-si|-ui|-f|-lf|-bool]");
			}
			
			/**** START DEBUG OUTPUT ****/
			if (debug) printf("Read\nmain: Address: 0x%X\nmain: Size: %lu\nBuffer: %p\n", address, read_size, buf);
			/***** END DEBUG OUTPUT *****/
			
			procmem_read(task, address, read_size, rw_mode);
			break;
		case _WRITE:
			if (data == NULL) {
				puts("Nothing to be written.");
				puts("Specify data to be written with [-b|-xi|-si|-ui|-f|-lf]");
				return 0;
			}
			
			procmem_write(task, address, data, rw_mode);
			break;
		case _LOCK:
			procmem_lock(task, address, lock_size);
			break;
		default:
			printf("Invalid mode.\n");
			break;
	}

	return 0;
}

// vim:ft=objc
