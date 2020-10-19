#include <substrate.h>
#include <pthread.h>
#include <unistd.h>

void *(*orig_calloc)(size_t size, size_t size2);
void *ret2 = NULL;
FILE *outfile;
FILE *f;
time_t thetime;
char filename[100];
Dl_info info;

void fromWhere(const char *str) {
fprintf(outfile, "%s is from %s\nImage base at %p\n%s is exactly at %p\n", str, info.dli_fname, info.dli_fbase, str, info.dli_saddr);
fflush(outfile);
}

void *hook_calloc(size_t size, size_t size2){

ret2 = calloc(size, size2);
fprintf(outfile, "%p:%lu:%lu\n", ret2, size, size2);

return ret2;
}

void *threadFunc(void *arg) {
fprintf(outfile, "Thread started.\n");
fflush(outfile);
while(1){
fflush(outfile);
sleep(3);
}
return NULL;
}


%ctor {
if((f = fopen("/var/mobile/hookalloclog/calloc", "r"))) {
thetime = time(NULL);
sprintf(filename, "/var/mobile/hookalloclog/%ld_Calloc.log", thetime);
outfile = fopen(filename, "a");

MSHookFunction((void *)0x66D9D0, (void *)hook_calloc, (void **)&orig_calloc);

pthread_t thread_id;
pthread_create(&thread_id, NULL, threadFunc, NULL);
pthread_detach(thread_id);

fprintf(outfile, "Start - calloc\n");
if(dladdr((void *)0x66D9D0, &info)) fromWhere("_calloc");
fclose(f);
}
}

__attribute__((destructor)) static void destroy_hookmalloc() {
fprintf(outfile, "End - calloc");
fclose(outfile);
NSLog(@"[HookCalloc] File closed successfully");
}