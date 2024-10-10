#include<stdio.h>
#include <stdlib.h>


int main() {
    
    char *data = (char *) malloc(sizeof(char)*10);
    if (data) {
        for (int i = 0; i < 10; ++i) {
            data[i] = (char) i;
        }
        free(data);
    }
    return 0;
}