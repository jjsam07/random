#import <Foundation/Foundation.h>
#import <mach/mach_traps.h>
#import <mach-o/dyld.h>
#import <mach/mach.h>
#import <substrate.h>
#import <dlfcn.h>
#include <libkern/OSCacheControl.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

uint32_t getSlide(const char *binary){
    for (uint32_t i = 0; i < _dyld_image_count(); i++){ // enumerate all images (i.e. executables and libs)
        const char *name = _dyld_get_image_name(i); // get full path of the image
        if (strstr(name, binary) != NULL){ 
            return _dyld_get_image_vmaddr_slide(i); //if found, return ASLR slide
        }
    }
    return 0; //return 0 if not found
}

char* infoProtectionLevel(vm_address_t address) {

    char message[] = "Memory protection: %c%c%c  %c%c%c";
    char *str = NULL;

    vm_size_t vmsize;
    vm_region_basic_info_data_t info;
    mach_msg_type_number_t info_count = VM_REGION_BASIC_INFO_COUNT;
    memory_object_name_t object;

    kern_return_t retval = vm_region(mach_task_self(), &address, &vmsize, VM_REGION_BASIC_INFO, (vm_region_info_t)&info, &info_count, &object);
    if (retval != KERN_SUCCESS){
        str = (char *)calloc(100, 1);
        sprintf(str, "Error: %s", mach_error_string(retval));
        return str;
    } else {
        str = (char *)calloc(sizeof(message), 1);
        sprintf(str, message,
            info.protection & VM_PROT_READ ? 'r' : '-',
            info.protection & VM_PROT_WRITE ? 'w' : '-',
            info.protection & VM_PROT_EXECUTE ? 'x' : '-',

            info.max_protection & VM_PROT_READ ? 'r' : '-',
            info.max_protection & VM_PROT_WRITE ? 'w' : '-',
            info.max_protection & VM_PROT_EXECUTE ? 'x' : '-'
        );
        return str;
    }

}

bool getRegionInfo(vm_address_t address, vm_region_basic_info_data_t *info) {

    vm_size_t vmsize;
    mach_msg_type_number_t info_count = VM_REGION_BASIC_INFO_COUNT;
    memory_object_name_t object;

    kern_return_t retval = vm_region(mach_task_self(), &address, &vmsize, VM_REGION_BASIC_INFO, (vm_region_info_t)info, &info_count, &object);
    if (retval != KERN_SUCCESS)
        return FALSE;

    return TRUE;

}

char* readMemory(vm_address_t address, size_t size){

    kern_return_t retval = 0;
    mach_port_t taskport = mach_task_self();
    unsigned char buf[size];
    size_t tempsize = size;
    char tmp[2];
    char *data = NULL;

    tmp[0] = '\0';

    retval = vm_read_overwrite(taskport, address, size, (vm_address_t)&buf, (vm_size_t *)&tempsize);
    if (retval != KERN_SUCCESS){
        data = (char *)calloc(100, 1);
        sprintf(data, "Error: %s", mach_error_string(retval));
        return data;
    } else {
        data = (char *)calloc(size*2, 1);
        for (int i = 0; i < size; i++){
            sprintf((char *)tmp, "%02hhX", buf[i]);
//printf("vm_read[%u]: [%d] = %02hhx\n", address+i, i, buf[i]);
            strcat(data, tmp);
        }
        return data;
    }
}

//******************************************//
//**Credits: HackJack & Razzile(Kamizoom)**//
//****************************************//

/*
 writes data to a memory address
 takes the address and the hex as a string as parameters
 */

// writeMemory() is a copy of writeData();

bool writeMemory(vm_address_t address,  string str) {
    
    //declaring variables
    
    kern_return_t err = 0;
    mach_port_t port = mach_task_self();
    vm_region_basic_info_data_t info;
    
    size_t find = str.find("0x");
    
    if (find != -1) {
        
        str.replace(find, 2, "");
    }
    
    int len = str.size();
    
    char data[len / 2];
    
    int x = 0;
    
    for(int i = 0; i < len; i += 2) {
        
        string dat = str.substr(i, 2);
        uint8_t tmp;
        sscanf(dat.c_str(), "%hhx", &tmp);
        data[x] = tmp;
        x++;
        
    }

    if (!getRegionInfo(address, &info))
        return FALSE;

//printf("info\n");
    //set memory protections to allow us writing code there
    
    err = vm_protect(port, (vm_address_t)address, sizeof(data), NO, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_COPY);
//printf("vm_protect\n");
    
    //check if the protection fails
    
    if (err != KERN_SUCCESS)
        return FALSE;
    
    //write code to memory
    //printf("%s\n", infoProtectionLevel(address));
    err = vm_write(port,address,(vm_address_t)&data,sizeof(data));
    //printf("vm_write\n");
    if (err != KERN_SUCCESS)
        return FALSE;
    
    /* Flush CPU data cache to save write to RAM - Thanks to c0deh4cker*/
	sys_dcache_flush((void *)address, sizeof(data));
//printf("sys_dcache_flush\n");
	/* Invalidate instruction cache to make the CPU read from RAM */
	sys_icache_invalidate((void *)address, sizeof(data));
//printf("sys_icache_invalidate\n");
    //set the protections back to normal so the app can access this address as usual
    
    err = vm_protect(port, (vm_address_t)address, sizeof(data), NO, info.protection);
//printf("vm_protect to normal\n");
    if (err != KERN_SUCCESS)
        return FALSE;
    
    return TRUE;
	
}
