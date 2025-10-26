#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/ipc.h>

typedef struct s {
    int a;
    float b;
} structutre;

int main(){

    int filas = 3;
    int columnas = 3;

    int id = shmget(IPC_PRIVATE, filas*columnas*sizeof(structutre)+ filas*sizeof(structutre*), 0666 | IPC_CREAT);
    void *base = shmat(id, NULL, 0);
    structutre **m = (structutre**)base;
    structutre *datos = (structutre*)((char*)base+filas*sizeof(structutre*));
    for(int i = 0; i<filas; i++){
        m[i]= &datos[i*columnas];
    }


    for(int i = 0; i < filas; i++){
        for(int j = 0; j < columnas; j++){
            structutre st = {
                .a = 10+i,
                .b = 2.12+j
            };
            m[i][j] = st;
        }
    }

    for(int i = 0; i < filas; i++){
        for(int j = 0; j < columnas; j++){
            printf("%d %f ", m[i][j].a, m[i][j].b);
        }
        printf("\n");
    }


    return 0;
}