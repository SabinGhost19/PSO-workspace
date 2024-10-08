#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

void * malloc(size_t size){

    static void *(*real_malloc)(size_t)=NULL;
    if(!real_malloc){
        real_malloc = dlsym(RTLD_NEXT, "malloc");
        if (!real_malloc) {
            printf("Error in `dlsym`: %s\n", dlerror());
            exit(1);
        }
    }
    void*p=real_malloc(size);
    printf("S-au alocat %zu octeti,la adresa de memorie %p",size,p);
    
    return p;
}

void free(void*ptr){

    static void (*real_free)(void *)=NULL;

    if(!real_free){
        real_free=dlsym(RTLD_NEXT,"free");
        if (!real_free) {
            printf("Error in `dlsym`: %s\n", dlerror());
            exit(1);
        }
    }
    real_free(ptr);
    printf("free memory at address %p\n", ptr);
}