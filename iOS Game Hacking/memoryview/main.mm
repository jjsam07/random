#include <stdlib.h>
#include <stdio.h>
#include <mach/mach.h>

int main(int argc, char **argv, char **envp) {
	pid_t pid;
	vm_address_t address;
	size_t size;
	mach_port_name_t task;
	kern_return_t res;
	int i = 0;
	
	if (argc < 4) {
		printf("Usage: %s [PID] [ADDRESS] [SIZE]\n", argv[0]);
		return 0;
	}
	
	pid = strtoul(argv[1], NULL, 10);
	address = strtoul(argv[2], NULL, 16);
	size = strtoul(argv[3], NULL, 10);
	unsigned char buf[size];
	
	if (address == 0) {
		printf("strtoul error???\n");
	}
	
	if (size == 0) {
		printf("Invalid size!\n");
		return 0;
	}
	
	res = task_for_pid(mach_task_self(), pid, &task);
	
	if (res != KERN_SUCCESS) {
		printf("%s\n", mach_error_string(res));
		return 0;
	}
	printf("All good!\n");
	
	while(1) {
		res = vm_read_overwrite(task, address, size, (vm_address_t)&buf, (vm_size_t *)&size);
		if (res != KERN_SUCCESS) {
			printf("Read error: %s\n", mach_error_string(res));
		}
		printf("0x%X: ", address);
		for (i = 0; i < size; i++) {
			//if ((i % 12) == 0 && i != 0) printf("\n0x%X: ", address+i);
			if ((i % 4) == 0 /*&& (i % 12) != 0*/) printf("  ");
			printf("%02hhX ", buf[i]);
			
		}
		printf("\r");
		fflush(stdout);
	}
	return 0;
}

// vim:ft=objc
