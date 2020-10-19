#ifndef _HOOKS_H
#define _HOOKS_H
/*   Global Variables   */
void *r0 = 0;
void *r1 = 0;
void *r2 = 0;
void *r3 = 0;
void *r4 = 0;
void *r5 = 0;
void *r6 = 0;
void *r7 = 0;
void *r8 = 0;
void *r9 = 0;
void *r10 = 0;
void *r11 = 0;
void *r12 = 0;
void *lr = 0;

unsigned int currentAmmo = 0;
unsigned int ammoIndex = 0;
unsigned int currentCarHealth = 0;

uintptr_t ammoData = 0x76CD5C;

//void (*sub_4B2F54)(); //weapon reload
//void (*sub_4A89F4)(); //health write
void (*sub_515038)(); //car health write (bullet/punch)
void (*sub_513F4C)(); //car health write (collision)
void (*sub_48A2F0)(); //collision parent 
void (*sub_4B1338)(); //bullet/punch parent
void (*origDamage)();
/*
void log_4B2F54(){ //weapon reload
	if(r1 != 0){
		//FILE *f = fopen("/var/mobile/weaponData.hex","wb+");
		//FILE *f2 = fopen("/var/mobile/weaponExtraData.hex","wb+");
		//FILE *f3 = fopen("/var/mobile/ammoData.hex","wb+");

		//index at 0x64

		ammoIndex = ((int*)r0)[25];
		currentAmmo = (((int*)r1))[ammoIndex + 28];

		//fwrite(r0, 1, 0xFF, f);
		//fwrite(r1, 1, 0xFF, f2);
		//fwrite((void*)(**(uintptr_t**)ammoData), 1, 0xFF, f3);

		//fclose(f);
		//fclose(f2);
		//fclose(f3);
	}
	NSLog(@"Reload weapon\nindex = %u/0x%X\ncurrentAmmo = %u/0x%X\nr0 = %p\nr1 = %p\nr2 = %p\nr3 = %p\nr4 = %p\nr5 = %p\nr6 = %p\nr7 = %p\nr8 = %p\nr9 = %p\nr10 = %p\nr11 = %p\nr12 = %p\nlr = %p\n", ammoIndex, ammoIndex, currentAmmo, currentAmmo, r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr);
}

void log_4A89F4(){ //health write
	NSLog(@"Health write\nr0 = %p\nr1 = %p\nr2 = %p\nr3 = %p\nr4 = %p\nr5 = %p\nr6 = %p\nr7 = %p\nr8 = %p\nr9 = %p\nr10 = %p\nr11 = %p\nr12 = %p\nlr = %p\n", r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr);
}
*/
void log_515038(){ //car health write (bullet/punch)
	currentCarHealth = ((int *)r0)[240];
	NSLog(@"Car health write (bullet/punch)\ncurrent health: %u/0x%x at %p\nr0 = %p\nr1 = %p\nr2 = %p\nr3 = %p\nr4 = %p\nr5 = %p\nr6 = %p\nr7 = %p\nr8 = %p\nr9 = %p\nr10 = %p\nr11 = %p\nr12 = %p\nlr = %p\n", currentCarHealth, currentCarHealth, (void *)((int)r0+0x3C0), r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr);
}

void log_513F4C(){ //car health write (collision)
	currentCarHealth = ((int *)r0)[240];
	NSLog(@"Car health write (collision)\ncurrent health: %u/0x%x at %p\nr0 = %p\nr1 = %p\nr2 = %p\nr3 = %p\nr4 = %p\nr5 = %p\nr6 = %p\nr7 = %p\nr8 = %p\nr9 = %p\nr10 = %p\nr11 = %p\nr12 = %p\nlr = %p\n", currentCarHealth, currentCarHealth, (void *)((int)r0+0x3C0), r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr);
}

void log_48A2F0(){ 
	NSLog(@"Car health write (collision parent)\nr0 = %p\nr1 = %p\nr2 = %p\nr3 = %p\nr4 = %p\nr5 = %p\nr6 = %p\nr7 = %p\nr8 = %p\nr9 = %p\nr10 = %p\nr11 = %p\nr12 = %p\nlr = %p\n", r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr);
}

void log_4B1338(){ 
	NSLog(@"Car health write (bullet/punch parent)\nr0 = %p\nr1 = %p\nr2 = %p\nr3 = %p\nr4 = %p\nr5 = %p\nr6 = %p\nr7 = %p\nr8 = %p\nr9 = %p\nr10 = %p\nr11 = %p\nr12 = %p\nlr = %p\n", r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr);
}
/*
void replace_4B2F54() __attribute__ ((optnone)){ //weapon reload
	//r0 - "weapon data" pointer
	asm volatile("push {r0-r7,lr}");
	asm volatile("cmp r1, #0");
	asm volatile("mov %0, r0":"=r" (r0));
	asm volatile("pop {r0-r7,lr}");
	asm volatile("push {r0-r7,lr}");
	asm volatile("mov %0, r1":"=r" (r1));
	asm volatile("mov %0, r2":"=r" (r2));
	asm volatile("mov %0, r3":"=r" (r3));
	asm volatile("mov %0, r4":"=r" (r4));
	asm volatile("mov %0, r5":"=r" (r5));
	asm volatile("mov %0, r6":"=r" (r6));
	asm volatile("mov %0, r7":"=r" (r7));
	asm volatile("mov %0, r8":"=r" (r8));
	asm volatile("mov %0, r9":"=r" (r9));
	asm volatile("mov %0, r10":"=r" (r10));
	asm volatile("mov %0, r11":"=r" (r11));
	asm volatile("mov %0, r12":"=r" (r12));
	asm volatile("mov %0, lr":"=r" (lr));
	asm volatile("mov r0, %0"::"r" (log_4B2F54));
	asm volatile("it ne");
	asm volatile("blx r0");
	asm volatile("mov r8, %0"::"r" (sub_4B2F54));
	asm volatile("pop {r0-r7,lr}");
	asm volatile("bx r8");
}

void replace_4A89F4() __attribute__ ((optnone)){ //health write
	//r0 - "health data" pointer
	asm volatile("push {r0-r11,lr}");
	asm volatile("mov %0, r0":"=r" (r0));
	asm volatile("pop {r0-r11,lr}");
	asm volatile("push {r0-r11,lr}");
	asm volatile("mov %0, r1":"=r" (r1));
	asm volatile("mov %0, r2":"=r" (r2));
	asm volatile("mov %0, r3":"=r" (r3));
	asm volatile("mov %0, r4":"=r" (r4));
	asm volatile("mov %0, r5":"=r" (r5));
	asm volatile("mov %0, r6":"=r" (r6));
	asm volatile("mov %0, r7":"=r" (r7));
	asm volatile("mov %0, r8":"=r" (r8));
	asm volatile("mov %0, r9":"=r" (r9));
	asm volatile("mov %0, r10":"=r" (r10));
	asm volatile("mov %0, r11":"=r" (r11));
	asm volatile("mov %0, r12":"=r" (r12));
	asm volatile("mov %0, lr":"=r" (lr));
	asm volatile("mov r0, %0"::"r" (log_4A89F4));
	asm volatile("blx r0");
	asm volatile("mov r12, %0"::"r" (sub_4A89F4));
	asm volatile("pop {r0-r11,lr}");
	asm volatile("bx r12");
}
*/
void replace_515038() __attribute__ ((optnone)){ //car health write (bullet/punch)
	asm volatile("push {r0-r11,lr}");
	asm volatile("mov %0, r0":"=r" (r0));
	asm volatile("pop {r0-r11,lr}");
	asm volatile("push {r0-r11,lr}");
	asm volatile("mov %0, r1":"=r" (r1));
	asm volatile("mov %0, r2":"=r" (r2));
	asm volatile("mov %0, r3":"=r" (r3));
	asm volatile("mov %0, r4":"=r" (r4));
	asm volatile("mov %0, r5":"=r" (r5));
	asm volatile("mov %0, r6":"=r" (r6));
	asm volatile("mov %0, r7":"=r" (r7));
	asm volatile("mov %0, r8":"=r" (r8));
	asm volatile("mov %0, r9":"=r" (r9));
	asm volatile("mov %0, r10":"=r" (r10));
	asm volatile("mov %0, r11":"=r" (r11));
	asm volatile("mov %0, r12":"=r" (r12));
	asm volatile("mov %0, lr":"=r" (lr));
	asm volatile("mov r0, %0"::"r" (log_515038));
	asm volatile("blx r0");
	asm volatile("mov r12, %0"::"r" (sub_515038));
	asm volatile("pop {r0-r11,lr}");
	asm volatile("bx r12");
}
/*
void replace_513F4C() __attribute__ ((optnone)){ //car health write (collision)
	asm volatile("push {r0-r11,lr}");
	asm volatile("mov %0, r0":"=r" (r0));
	asm volatile("pop {r0-r11,lr}");
	asm volatile("push {r0-r11,lr}");
	asm volatile("mov %0, r1":"=r" (r1));
	asm volatile("mov %0, r2":"=r" (r2));
	asm volatile("mov %0, r3":"=r" (r3));
	asm volatile("mov %0, r4":"=r" (r4));
	asm volatile("mov %0, r5":"=r" (r5));
	asm volatile("mov %0, r6":"=r" (r6));
	asm volatile("mov %0, r7":"=r" (r7));
	asm volatile("mov %0, r8":"=r" (r8));
	asm volatile("mov %0, r9":"=r" (r9));
	asm volatile("mov %0, r10":"=r" (r10));
	asm volatile("mov %0, r11":"=r" (r11));
	asm volatile("mov %0, r12":"=r" (r12));
	asm volatile("mov %0, lr":"=r" (lr));
	asm volatile("mov r0, %0"::"r" (log_513F4C));
	asm volatile("blx r0");
	asm volatile("mov r12, %0"::"r" (sub_513F4C));
	asm volatile("pop {r0-r11,lr}");
	asm volatile("bx r12");
}

void replace_48A2F0() __attribute__ ((optnone)){ //collision parent 
	asm volatile("push {r0-r11,lr}");
	asm volatile("mov %0, r0":"=r" (r0));
	asm volatile("pop {r0-r11,lr}");
	asm volatile("push {r0-r11,lr}");
	asm volatile("mov %0, r1":"=r" (r1));
	asm volatile("mov %0, r2":"=r" (r2));
	asm volatile("mov %0, r3":"=r" (r3));
	asm volatile("mov %0, r4":"=r" (r4));
	asm volatile("mov %0, r5":"=r" (r5));
	asm volatile("mov %0, r6":"=r" (r6));
	asm volatile("mov %0, r7":"=r" (r7));
	asm volatile("mov %0, r8":"=r" (r8));
	asm volatile("mov %0, r9":"=r" (r9));
	asm volatile("mov %0, r10":"=r" (r10));
	asm volatile("mov %0, r11":"=r" (r11));
	asm volatile("mov %0, r12":"=r" (r12));
	asm volatile("mov %0, lr":"=r" (lr));
	asm volatile("mov r0, %0"::"r" (log_48A2F0));
	asm volatile("blx r0");
	asm volatile("mov r12, %0"::"r" (sub_48A2F0));
	asm volatile("pop {r0-r11,lr}");
	asm volatile("bx r12");
}
*/
void replace_4B1338() __attribute__ ((optnone)){ //bullet/punch parent
	asm volatile("push {r0-r11,lr}");
	asm volatile("mov %0, r0":"=r" (r0));
	asm volatile("pop {r0-r11,lr}");
	asm volatile("push {r0-r11,lr}");
	asm volatile("mov %0, r1":"=r" (r1));
	asm volatile("mov %0, r2":"=r" (r2));
	asm volatile("mov %0, r3":"=r" (r3));
	asm volatile("mov %0, r4":"=r" (r4));
	asm volatile("mov %0, r5":"=r" (r5));
	asm volatile("mov %0, r6":"=r" (r6));
	asm volatile("mov %0, r7":"=r" (r7));
	asm volatile("mov %0, r8":"=r" (r8));
	asm volatile("mov %0, r9":"=r" (r9));
	asm volatile("mov %0, r10":"=r" (r10));
	asm volatile("mov %0, r11":"=r" (r11));
	asm volatile("mov %0, r12":"=r" (r12));
	asm volatile("mov %0, lr":"=r" (lr));
	asm volatile("mov r0, %0"::"r" (log_4B1338));
	asm volatile("blx r0");
	asm volatile("mov r12, %0"::"r" (sub_4B1338));
	asm volatile("pop {r0-r11,lr}");
	asm volatile("bx r12");
}

void hackDamage() __attribute__ ((optnone)){
	asm volatile("push {r0-r11,lr}");
	asm volatile("mov r12, %0"::"r" (origDamage));
	asm volatile("mov r1, %0"::"r" (damage));
	asm volatile("cmp r1, #1");
	asm volatile("pop {r0-r11,lr}");
	asm volatile("it eq");
	asm volatile("mov r5, #0x7FFFFFFF");
	asm volatile("bx r12");
}
#endif