
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
int leerNumeros(char * filename, int ** vec);
int sumaArchivo(char * filename);
int main(){
    int * vec;
    int totalnumeros=leerNumeros("entrada.txt",&vec);
    int suma = sumaArchivo("entrada.txt");
    printf("suma archivo: %d \n", suma);
    int acomulador =0;
    for (int j =0; j < totalnumeros ;j++){
        acomulador += vec[j];
    }
    printf("resultado: %d \n", acomulador);


    return 0;
}
int sumaArchivo(char * filename){
    FILE *f;
    f=fopen(filename,"r");
    if(f==NULL){
        perror("error al abrir el archivo");
        exit(-1);
    }
    int suma=0;
    int numero;
    int i=0;
    while(i<6){
        fscanf(f,"%d",&numero);
        suma+=numero;
        i++;
    }
    fclose(f);
    return suma;

}
int leerNumeros(char * filename, int ** vec){
    FILE * f;
    f=fopen(filename,"r");
    if (f==NULL){
        perror("Error al abrir el archivo");
        exit(-1);   
    }
    int totalnumeros, numero;
    fscanf(f,"%d",&totalnumeros);
    *vec = (int*) calloc(totalnumeros, sizeof(int));
    for(int i =0; i<totalnumeros; i++){
        fscanf(f,"%d",&numero);
        (*vec)[i]=numero;

    }
    return totalnumeros;
}
