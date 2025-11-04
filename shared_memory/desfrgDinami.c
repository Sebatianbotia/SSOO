#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<signal.h>

void log_matrix(int** M, int f, int c){
    for(int i = 0; i < f; i++){
        for(int j = 0; j < c; j++){
            printf("%d ", M[i][j]);
        }
        printf("\n");
    }
}

int main(){
    FILE * fd = fopen("dsfdinamica.txt",  "r");
    int filas, columnas;

    fscanf(fd, "%d", &filas);
    fscanf(fd, "%d", &columnas);
    //Mat O

    int ** O = (int **)malloc(sizeof(int *)*filas);
    if(!O){
        perror("memory");
        abort();
    }
    for(int i =0; i < filas; i++){
        O[i] = (int*)malloc(sizeof(int)*columnas);
    }
    for(int i =0; i<filas; i++){
        for(int j = 0 ; j<columnas;j++){
            fscanf(fd, "%d", &O[i][j]);
        }
    }
    //Mat R
    int ** R = (int **)malloc(sizeof(int *)*filas);
    if(!R){
        perror("memory");
        abort();
    }
    for(int i =0; i < filas; i++){
        R[i] = (int*)malloc(sizeof(int)*columnas);
    }
    for(int i =0; i<filas; i++){
        for(int j = 0 ; j<columnas;j++){
            fscanf(fd, "%d", &R[i][j]);
        }
    }



    
    //Mat T 
    int ** T = (int **)malloc(sizeof(int *)*filas);
    if(!T){
        perror("memory");
        abort();
    }
    for(int i =0; i < filas; i++){
        T[i] = (int*)malloc(sizeof(int)*columnas);
    }
    for(int i =0; i<filas; i++){
        for(int j = 0 ; j<columnas;j++){
            fscanf(fd, "%d", &T[i][j]);
        }
    }

    log_matrix(O, filas ,columnas);
    printf("\n");
    log_matrix(R, filas ,columnas);
    printf("\n");
    log_matrix(T, filas ,columnas);

    

    return 0;




}