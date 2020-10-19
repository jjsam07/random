//******************************************//
//**Credits: HackJack & Razzile(Kamizoom)**//
//****************************************//

//********************************************//
//**Usage: writeData(0xOFFSET, "Hex Data"); **//
//******************************************//

//importing and including files

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

/*
 writes data to a memory address
 takes the address and the hex as a string as parameters
 */

bool writeData(vm_address_t address,  string str) {
    
    //declaring variables
    
    kern_return_t err = 0;
    mach_port_t port = mach_task_self();
    
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
    
    //set memory protections to allow us writing code there
    
    err = vm_protect(port, (vm_address_t)address, sizeof(data), NO, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_COPY);
    
    //check if the protection fails
    
    if (err != KERN_SUCCESS)
        return FALSE;
    
    //write code to memory
    
    err = vm_write(port,address,(vm_address_t)&data,sizeof(data));
    
    if (err != KERN_SUCCESS)
        return FALSE;
    
    /* Flush CPU data cache to save write to RAM - Thanks to c0deh4cker*/
	sys_dcache_flush((void *)address, sizeof(data));
	/* Invalidate instruction cache to make the CPU read from RAM */
	sys_icache_invalidate((void *)address, sizeof(data));
    
    //set the protections back to normal so the app can access this address as usual
    
    err = vm_protect(port, (vm_address_t)address, sizeof(data), NO, VM_PROT_READ | VM_PROT_EXECUTE);
    
    if (err != KERN_SUCCESS)
        return FALSE;
    
    return TRUE;
	
}
