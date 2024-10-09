#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

#define BUFFER_SIZE 10

int main(int argc,char*argv[]){
    
    int fd;
    char buffer[BUFFER_SIZE];
    char*buffer2="SABIN";
    ssize_t bytesWriten;

    fd=open("f1.txt",O_RDWR| O_FSYNC,0644);
    if(fd<0){
        perror("Error opening file 1");
        exit(-1);
    }

    ssize_t bytesRead;
    bytesRead=read(fd,buffer,BUFFER_SIZE);
    if (bytesRead == -1) {
        perror("Failed to read from file");
        close(fd);
        exit(-1);
    }
    
    bytesWriten=write(STDOUT_FILENO,buffer,bytesRead); 
    
    if(bytesWriten==-1){
        perror("Failed to write data");
        exit(-1);
    }    

    bytesRead=0;
    bytesRead=write(fd,buffer2,5);

    if(bytesWriten==-1){
        perror("Failed to write data");
        close(fd);
        exit(-1);
    }    

    close(fd);

    int fo;
    fo=open("fisier_nou.txt",O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
    
    if(close(fo)==-1){
        perror("Failed to close file");
        exit(-1);
    }

    return 0;
}