#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<signal.h>

int ** read_file( const char * fName, int *f, int * c){
    FILE * fd = fopen(fName,  "r");
    if(!fd){
        perror("file");
        abort();

    }
    fscanf(fd, "%d", f);
    fscanf(fd, "%d", c);

    int ** mat = (int **)malloc(sizeof(int *)*(*f));
    if(!mat){
        perror("memory");
        abort();
    }
    for(int i =0; i < *f; i++){
        mat[i] = (int*)malloc(sizeof(int)*(*c));
    }


    for(int i =0; i<*f; i++){
        for(int j = 0 ; j<*c;j++){
            fscanf(fd, "%d", &mat[i][j]);
        }
    }
    fclose(fd); 
    return mat;
}

void move_mat(int ** origin, int** destino, int f, int c){
    for(int i =0; i<f;i++){
        for(int j=0;j<c;j++){
            destino[i][j]=origin[i][j];
        }
    }
}

void log_matrix(int** M, int f, int c){
    for(int i = 0; i < f; i++){
        for(int j = 0; j < c; j++){
            printf("%d ", M[i][j]);
        }
        printf("\n");
    }
}

void multiplicarMatricesHijo1(int **A, int **B, int **resultado, int filas, int columnas, pid_t pid){
    for(int i=0; i<filas; i++){
        for(int j = 0; j<columnas; j++ ){
            if(i==0||i==filas-1){
                resultado[i][j] = 0;
                for(int k=0;k<filas;k++){
                    resultado[i][j] += A[i][k]* B[k][j];
                }
                printf("hijo: %d, calculo de posicion: [%d][%d]\n", pid, i, j);
            }else{
                if(j==0||j==columnas-1){
                resultado[i][j] = 0;
                for(int k=0;k<filas;k++){
                    resultado[i][j] += A[i][k] * B[k][j];

                }
                printf("hijo: %d, calculo de posicion: [%d][%d]\n", pid, i, j);

                }
         
            }
        }
    }
}

void multiplicarMatricesHijo2(int **A, int **B, int **resultado, int filas, int columnas, pid_t pid){
    for(int i=1; i<filas-1; i++){
        for(int j = 1; j<columnas-1; j++ ){
            if(i==1||i==filas-2){
                resultado[i][j] = 0;
                for(int k=0;k<filas;k++){
                    resultado[i][j] += A[i][k]* B[k][j];       
                }
                printf("hijo: %d, calculo de posicion: [%d][%d]\n", pid, i, j);
            }else{
                if(j==1||j==columnas-2){
                resultado[i][j] = 0;
                for(int k=0;k<filas;k++){
                    resultado[i][j] += A[i][k] * B[k][j];     
                }
                printf("hijo: %d, calculo de posicion: [%d][%d]\n", pid, i, j);
                }
         
            }
        }
    }
}

void multiplicarMatricesHijo3(int **A, int **B, int **resultado, int filas, int columnas, pid_t pid){
    for(int i=2; i<filas-2; i++){
        for(int j = 2; j<columnas-2; j++ ){
                resultado[i][j] = 0;
                for(int k=0;k<filas;k++){
                    resultado[i][j] += A[i][k]* B[k][j];
                }
                printf("hijo: %d, calculo de posicion: [%d][%d]\n", pid, i, j);

            }
    }
}


int main(){
    int filas, columnas, horas, nHijos;
    int ** matA = read_file("matSnailA.txt",&filas, &columnas);
    int ** matB = read_file("matSnailB.txt",&filas, &columnas);


    //matriz principal
    int shmidPR = shmget(IPC_PRIVATE,sizeof(int)*filas*columnas + sizeof(int*)*filas, 0666| IPC_CREAT);
    void * base = shmat(shmidPR, NULL, 0);
    int ** principal = (int**)base;
    int * datos = (int*)((char*)base + sizeof(int*)*filas);
    for(int i = 0; i<filas; i++){
        principal[i] = &datos[i*columnas];
    }


    log_matrix(matA, filas ,columnas);

    printf("\n");
    log_matrix(matB, filas ,columnas);
    //creacion de hijos

    pid_t root = getpid();
    int i = 0;
    for(;i<3;i++){
        if(!fork()){
            break;
        }    
    }

    if(root == getpid()){
        printf("esperando que mis hijos terminen de hacer las operaciones...\n");
        for(int j = 0; j<3; j++){
            wait(NULL);
        }
        log_matrix(principal, filas, columnas);
    }
    else{
        if(i==0){
            multiplicarMatricesHijo1(matA, matB, principal, filas, columnas, getpid());
        }
        else if( i==1){
            multiplicarMatricesHijo2(matA, matB, principal, filas, columnas, getpid());

        }
        else{
            multiplicarMatricesHijo3(matA, matB, principal, filas, columnas, getpid());

        }
    }
    
    return 0;
}