#include <stdlib.h>
#include <stdio.h>
#include <mach/mach.h>
#include <sys/sysctl.h>
#include "GameObjects.h"

pid_t get_pid_by_name(char *process_name) {
	size_t sysctl_size;
	struct kinfo_proc *process_list = NULL;
	int sysctl_status;
	pid_t pid;

	int sysctl_mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
	
	sysctl_status = sysctl(sysctl_mib, 4, NULL, &sysctl_size, NULL, 0);
	process_list = malloc(sysctl_size);
	sysctl_status = sysctl(sysctl_mib, 4, process_list, &sysctl_size, NULL, 0);
	
	for(int i = 0; i < sysctl_size/sizeof(struct kinfo_proc); i++) {
		if (strncmp(process_list[i].kp_proc.p_comm, process_name, MAXCOMLEN) == 0) {
			pid = process_list[i].kp_proc.p_pid;
			free(process_list);
			return pid;
		}
	}
	
	free(process_list);
	
	return 0;
}

int main(int argc, char *argv[], char *envp[]) {
	pid_t pid = get_pid_by_name("GangStarFreemium");
	kern_return_t res;
	mach_port_t task;
	vm_size_t size;
	float x, y, z;
	
	void *player_ptr = NULL;
	void *position0_ptr = NULL;
	void *position_ptr = NULL;
	void *position_ptr_ptr = NULL;
	
	if (argc < 2) {
		printf("Missing coordinates.\n");
		return 0;
	}
	x = strtof(argv[1], NULL) / 100.0f;
	y = strtof(argv[2], NULL) / 100.0f;
	z = strtof(argv[3], NULL);
	
	
	res = task_for_pid(mach_task_self(), pid, &task);
	if (res != KERN_SUCCESS) {
		printf("task_for_pid: %d: %s\n", pid, mach_error_string(res));
		return 0;
	}
	
	size = 4;
	res = vm_read_overwrite(task, (vm_address_t)0xB0DF84, size, (vm_address_t)&player_ptr, &size);
	if (res != KERN_SUCCESS) {
		printf("player_ptr: vm_read_overwrite: %s\n", mach_error_string(res));
		return 0;
	}
	
	size = 4;
	res = vm_read_overwrite(task, (vm_address_t)0xB09420, size, (vm_address_t)&position_ptr_ptr, &size);
	if (res != KERN_SUCCESS) {
		printf("position_ptr_ptr: vm_read_overwrite: %s\n", mach_error_string(res));
		return 0;
	}
	
	size = 4;
	res = vm_read_overwrite(task, ((vm_address_t)player_ptr)+0x14, size, (vm_address_t)&position0_ptr, &size);
	if (res != KERN_SUCCESS) {
		printf("position0_ptr: vm_read_overwrite: %s\n", mach_error_string(res));
		return 0;
	}
	
	size = 4;
	res = vm_read_overwrite(task, ((vm_address_t)position_ptr_ptr)+0x44, size, (vm_address_t)&position_ptr, &size);
	if (res != KERN_SUCCESS) {
		printf("position_ptr: vm_read_overwrite: %s\n", mach_error_string(res));
		return 0;
	}
	
	res = vm_write(task, ((vm_address_t)position0_ptr)+0x48, (vm_address_t)&z, sizeof(float));
	if (res != KERN_SUCCESS) {
		printf("position0_ptr: vm_write: %s\n", mach_error_string(res));
		exit(0);
	}
	
	res = vm_write(task, ((vm_address_t)position_ptr)+0x2C, (vm_address_t)&x, sizeof(float));
	if (res != KERN_SUCCESS) {
		printf("position_ptr X: vm_write: %s\n", mach_error_string(res));
		exit(0);
	}
	
	res = vm_write(task, ((vm_address_t)position_ptr)+0x30, (vm_address_t)&y, sizeof(float));
	if (res != KERN_SUCCESS) {
		printf("position_ptr Y: vm_write: %s\n", mach_error_string(res));
		exit(0);
	}
	
	return 0;
}
