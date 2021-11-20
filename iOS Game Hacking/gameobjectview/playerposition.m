#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <mach/mach.h>
#include <sys/sysctl.h>
#include "GameObjects.h"

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
	int ch;
	
	void *player_ptr = NULL;
	void *position0_ptr = NULL;
	void *position1_ptr = NULL;
	PlayerPosition0 *pos0 = malloc(sizeof(PlayerPosition0));
	PlayerPosition1 *pos1 = malloc(sizeof(PlayerPosition1));
	float position[3];
	
	
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
	res = vm_read_overwrite(task, ((vm_address_t)player_ptr)+0x14, size, (vm_address_t)&position0_ptr, &size);
	if (res != KERN_SUCCESS) {
		printf("position0_ptr: vm_read_overwrite: %s\n", mach_error_string(res));
		return 0;
	}
	
	size = 4;
	res = vm_read_overwrite(task, ((vm_address_t)player_ptr)+0x2F8, size, (vm_address_t)&position1_ptr, &size);
	if (res != KERN_SUCCESS) {
		printf("position1_ptr: vm_read_overwrite: %s\n", mach_error_string(res));
		return 0;
	}
	

	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	
	while(1) {
		
		size = sizeof(PlayerPosition0);
		res = vm_read_overwrite(task, ((vm_address_t)position0_ptr), size, (vm_address_t)pos0, &size);
		if (res != KERN_SUCCESS) {
			printf("pos0: vm_read_overwrite: %s\n", mach_error_string(res));
			return 0;
		}
		
		size = sizeof(PlayerPosition1);
		res = vm_read_overwrite(task, ((vm_address_t)position1_ptr), size, (vm_address_t)pos1, &size);
		if (res != KERN_SUCCESS) {
			printf("pos1: vm_read_overwrite: %s\n", mach_error_string(res));
			return 0;
		}
		
		size = 0xC;
		res = vm_read_overwrite(task, ((vm_address_t)player_ptr)+0x7E4, size, (vm_address_t)&position, &size);
		if (res != KERN_SUCCESS) {
			printf("position: vm_read_overwrite: %s\n", mach_error_string(res));
			return 0;
		}
		
		printw("PlayerObject -> pos0 (%p):\n\nX (%p): %f\nY (%p): %f\nZ (%p): %f\n\nX (%p): %f\nY (%p): %f\nZ (%p): %f\n\nX (%p): %f\nY (%p): %f\nZ (%p): %f\n\n\nPlayerObject -> pos1 (%p):\n\nX (%p): %f\nY (%p): %f\nZ (%p): %f\n\n\nPlayerObject -> x, y, z (%p):\n\nX (%p): %f\nY (%p): %f\nZ (%p): %f\n", position0_ptr, position0_ptr+0x40, pos0->XYZ_0[0], position0_ptr+0x44, pos0->XYZ_0[1], position0_ptr+0x48, pos0->XYZ_0[2], position0_ptr+0x84, pos0->XYZ_1[0], position0_ptr+0x88, pos0->XYZ_1[1], position0_ptr+0x8C, pos0->XYZ_1[2], position0_ptr+0x98, pos0->XYZ_2[0], position0_ptr+0x9C, pos0->XYZ_2[1], position0_ptr+0xA0, pos0->XYZ_1[2], position1_ptr, position1_ptr+0x40, pos1->x, position1_ptr+0x44, pos1->y, position1_ptr+0x48, pos1->z, player_ptr, (player_ptr)+0x7E4, position[0], (player_ptr)+0x7E8, position[1], (player_ptr)+0x7EC, position[2]);
		
		refresh();
		usleep(250000);
		clear();
		ch = getch();
		if (ch == '\n') break;
	}
	
	endwin();
	free(pos0);
	free(pos1);
	
	return 0;
}
