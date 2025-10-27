#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#define BUFFER_SIZE 1024

typedef struct User {
    char* nombre;
    char* apellido;
    int edad;
} User;

int main(){

    int fd[2];
    pipe(fd);

    if(fork()){
        User u;
        close(fd[0]);
        //char* n = "Ivan";
        char buff[1024];
        u.nombre = (char*)malloc(4);
        u.apellido = (char*)malloc(9);
        strcpy(u.nombre, "Ivan");
        strcpy(u.apellido, "Ponchera");
        u.edad = 40;
        int size = strlen(u.nombre);
        printf("padre: %d\n", size);
        //write(fd[1], &size, sizeof(int));
        write(fd[1], u.nombre, size+1);
        wait(NULL);
    }else{
        close(fd[1]);
        User u;
        int size;
        char buff[1024];
        //read(fd[0], &size, sizeof(int));
        //printf("%d\n", size);
        read(fd[0], buff, sizeof(buff));
        printf("%d\n", strlen(buff));
        //buff[strlen(buff)] = '\0';
        u.nombre = (char*)malloc(strlen(buff));
        strcpy(u.nombre, buff);
        printf("..\n");
        printf("%s\n",u.nombre);
    }

    return 0;
}