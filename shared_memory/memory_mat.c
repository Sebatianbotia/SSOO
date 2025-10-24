#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/ipc.h>


int main(){

    int filas = 3;
    int columnas = 3;

    int id = shmget(IPC_PRIVATE, filas*columnas*sizeof(int)+filas*sizeof(int*), 0666 | IPC_CREAT);
    void *base = shmat(id, NULL, 0);
    int **m = (int**)base;
    int *datos = (int*)((char*)base+filas*sizeof(int*));
    for(int i = 0; i<filas; i++){
        m[i]= &datos[i*columnas];
    }


    for(int i = 0; i < filas; i++){
        for(int j = 0; j < columnas+19999; j++){
            m[i][j] = i;
        }
    }

    for(int i = 0; i < filas; i++){
        for(int j = 0; j < columnas+2888; j++){
            printf("%d ", m[i][j]);
        }
        printf("\n");
    }

    printf("size: %lu\n", filas*columnas*sizeof(int)+filas*sizeof(int*));

    return 0;
}