#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include<string.h>

void * malloc(size_t size){

    static void *(*real_malloc)(size_t)=NULL;
    char*buffer=NULL;
    if(!real_malloc){
        real_malloc = dlsym(RTLD_NEXT, "malloc");
        if (!real_malloc) {
            printf("Error in `dlsym`: %s\n", dlerror());
            exit(1);
        }
    }

    void*p=real_malloc(size);
    int bytes=sprintf(buffer,"S-au alocat %zu octeti,la adresa de memorie %p",size,p);
    write(1, buffer, bytes);
    return p;
}

void free(void*ptr){

    static void (*real_free)(void *)=NULL;
    char*buffer=NULL;

    if(!real_free){
        real_free=dlsym(RTLD_NEXT,"free");
        if (!real_free) {
            printf("Error in `dlsym`: %s\n", dlerror());
            exit(1);
        }
    }
    real_free(ptr);
    int bytes=sprintf(buffer,"free memory at address %p\n", ptr);
    write(1, buffer, bytes);
}