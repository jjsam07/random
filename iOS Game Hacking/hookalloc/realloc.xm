#include <substrate.h>
#include <pthread.h>
#include <unistd.h>

void *(*orig_realloc)(void *ptr, size_t size);
void *ret3 = NULL;
FILE *outfile;
FILE *f;
time_t thetime;
char filename[100];
Dl_info info;

void fromWhere(const char *str) {
fprintf(outfile, "%s is from %s\nImage base at %p\n%s is exactly at %p\n", str, info.dli_fname, info.dli_fbase, str, info.dli_saddr);
fflush(outfile);
}

void *hook_realloc(void *ptr, size_t size){

ret3 = realloc(ptr, size);
fprintf(outfile, "%p:%lu\n", ret3, size);

return ret3;
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
if ((f = fopen("/var/mobile/hookalloclog/realloc", "r"))) {
thetime = time(NULL);
sprintf(filename, "/var/mobile/hookalloclog/%ld_Realloc.log", thetime);
outfile = fopen(filename, "a");

MSHookFunction((void *)0x66E320, (void *)hook_realloc, (void **)&orig_realloc);

pthread_t thread_id;
pthread_create(&thread_id, NULL, threadFunc, NULL);
pthread_detach(thread_id);

fprintf(outfile, "Start - realloc\n");
if(dladdr((void *)0x66E320, &info)) fromWhere("_realloc");
fclose(f);
}
}

__attribute__((destructor)) static void destroy_hookmalloc() {
fprintf(outfile, "End - realloc");
fclose(outfile);
NSLog(@"[HookRealloc] File closed successfully");
}