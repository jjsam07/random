#include <substrate.h>
#include <pthread.h>
#include <unistd.h>

void *(*orig_malloc)(size_t size);
void *ret1 = NULL;
FILE *outfile;
FILE *f;
time_t thetime;
char filename[100];
Dl_info info;
unsigned int theindex = 0;
void *buf = NULL;

void fromWhere(const char *str) {
fprintf(outfile, "%s is from %s\nImage base at %p\n%s is exactly at %p\n", str, info.dli_fname, info.dli_fbase, str, info.dli_saddr);
fflush(outfile);
}
extern "C" {
void hook_malloc() __attribute__ ((optnone)) {
asm volatile("push {r1-r3}");
asm volatile("push {r0}");
asm volatile("mov r1, %0"::"r" (&theindex));
asm volatile("mov r2, %0"::"r" (buf));
asm volatile("ldr r3, [r1]");
asm volatile("pop {r0}");
asm volatile("str r0, [r2, r3]");
asm volatile("push {r1-r3}");
asm volatile("blx _malloc");
asm volatile("pop {r1-r3}");
asm volatile("add r3, r3, #4");
asm volatile("str r0, [r2, r3]");
asm volatile("str r3, [r1]");
asm volatile("pop {r1-r3}");
//asm volatile("bx lr");
}
}
void *threadFunc(void *arg) {
fprintf(outfile, "Thread started.\n");
fflush(outfile);
sleep(10);
MSHookFunction((void *)0x66DFC0, (void *)hook_malloc, (void **)&orig_malloc);
while(1){
sleep(3);
fwrite(buf, 1, theindex, outfile);
fflush(outfile);
theindex = 0;
}
return NULL;
}


%ctor {
if ((f = fopen("/var/mobile/hookalloclog/malloc", "r"))) {
thetime = time(NULL);
sprintf(filename, "/var/mobile/hookalloclog/%ld_Malloc.log", thetime);
outfile = fopen(filename, "ab");

buf = malloc(5000000);

if(buf == NULL) fprintf(outfile, "Can't allocate memory\n");

pthread_t thread_id;
pthread_create(&thread_id, NULL, threadFunc, NULL);
pthread_detach(thread_id);

fprintf(outfile, "Start - malloc\n");
fflush(outfile);
if(dladdr((void *)0x66DFC0, &info)) fromWhere("_malloc");
fclose(f);
}
}

__attribute__((destructor)) static void destroy_hookmalloc() {
fprintf(outfile, "End - malloc");
fclose(outfile);
NSLog(@"[HookMalloc] File closed successfully");
}