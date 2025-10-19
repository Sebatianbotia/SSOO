#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>

void leerArchivo(void **matriz);

int main(){
    int filas, columnas;
    int **matriz;
    FILE *archivo = fopen("matriz.txt", "r");
    if (archivo==NULL){
        perror("error al leer el archivo");
        exit(-1);
    }
    fscanf(archivo, "%d" , &filas);
    fscanf(archivo, "%d" , &columnas);
    printf("Matriz %d x %d ", filas, columnas);

    matriz = (int**)malloc(filas*sizeof(int*));// aca reservamos memoria para el vector 
                                                // de punteros que van a apuntar a los respectivos vectores (columnaas)
    for(int i = 0; i<filas;i++){
        matriz[i]=(int*)malloc(columnas*sizeof(int));//aca basicamente reservamos memoria para los vectores a los que                                               //apunta cada puntero en el vector de punteros
    }
    for(int i = 0; i < filas;i++ ){
        for(int j=0 ; j< columnas; j++){
            fscanf(archivo, "%d", &matriz[i][j]);//leemos matriz fila por fila;
            printf("[%d]", matriz[i][j]);//imprimimos matriz
        }
        printf("\n");
    }
    
    return 0;
}
