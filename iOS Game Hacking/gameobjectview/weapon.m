#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <mach/mach.h>
#include <sys/sysctl.h>
#include "GameObjects.h"
#include "procmem_read.h"

bool debug = false;

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
	procmem_rw_mode read_mode = _RW_MODE_NULL;
	
	if (argc < 2) return 0;
	
	if (strcmp(argv[1], "-b") == 0) {
		read_mode = _HEX_RAW;
	}
	else if (strcmp(argv[1], "-xi") == 0) {
		read_mode = _HEX_INT;
	}
	else if (strcmp(argv[1], "-si") == 0) {
		read_mode = _SIGNED_INT;
	}
	else if (strcmp(argv[1], "-ui") == 0) {
		read_mode = _UNSIGNED_INT;
	}
	else if (strcmp(argv[1], "-f") == 0) {
		read_mode = _FLOAT;
	}
	else if (strcmp(argv[1], "-lf") == 0) {
		read_mode = _DOUBLE;
	}
	else if (strcmp(argv[1], "-bool") == 0) {
		read_mode = _BOOLEAN;
	}
	if (argc > 2) {
		if (strcmp(argv[2], "-debug") == 0) {
			debug = true;
		}
	}
	
	void *player_ptr = NULL;
	void *inventory_ptr = NULL;
	WeaponInventory *inventory = malloc(0xAC);
	Weapon *weapon = malloc(0xAC);
	int weapon_type;
	
	
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
	res = vm_read_overwrite(task, ((vm_address_t)player_ptr)+0x814, size, (vm_address_t)&inventory_ptr, &size);
	if (res != KERN_SUCCESS) {
		printf("inventory_ptr: vm_read_overwrite: %s\n", mach_error_string(res));
		return 0;
	}
	
	size = 0xAC;
	res = vm_read_overwrite(task, (vm_address_t)inventory_ptr, size, (vm_address_t)inventory, &size);
	if (res != KERN_SUCCESS) {
		printf("inventory: vm_read_overwrite: %s\n", mach_error_string(res));
		return 0;
	}

	initscr();
	cbreak();
	noecho();
	
	while(1) {
		size = 4;
		res = vm_read_overwrite(task, ((vm_address_t)inventory_ptr)+0xA4, size, (vm_address_t)&weapon_type, &size);
		if (res != KERN_SUCCESS) {
			printf("weapon_type: vm_read_overwrite: %s\n", mach_error_string(res));
			return 0;
		}
		
		size = 0xAC;
		res = vm_read_overwrite(task, (vm_address_t)inventory->weapon[weapon_type], size, (vm_address_t)weapon, &size);
		if (res != KERN_SUCCESS) {
			printf("weapon: vm_read_overwrite: %s\n", mach_error_string(res));
			return 0;
		}
		
		printw("current weapon: %s\nweapon index: %d\nweapon clip: %d\n\n", weapon_name[weapon_type], weapon_type, weapon->weapon_clip);
		procmem_read((vm_address_t)inventory->weapon[weapon_type], (void *)weapon, 0xAC, read_mode);
		
		refresh();
		usleep(250);
		clear();
	}
	
	endwin();
	
	return 0;
}
