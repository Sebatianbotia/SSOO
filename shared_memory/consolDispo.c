#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<signal.h>
#include<string.h>



typedef struct Regiones{
    int R1;
    int R2;
    int R3;
} Regiones;

int main(int argc, int **argv){
    int nArchivos;
    int cantidad=0;
    FILE *fd = fopen("fnt.txt", "r");
    if(!fd){
        perror("file");
        return 1;
    }
    fscanf(fd, "%d", &nArchivos);

    char ** archNames;
    archNames = (char**)malloc(sizeof(char*)*nArchivos);
     

    char bff[1024];
    for(int i = 0; i<nArchivos;i++){
        fscanf(fd, "%s", bff);
        bff[strlen(bff)] = '\0';
        archNames[i] = malloc(strlen(bff));
        strcpy(archNames[i], bff);
        cantidad +=strlen(archNames[i]);
        memset(bff, '\0', 1);
    }


    int shmidStruct = shmget(IPC_PRIVATE, sizeof(Regiones), 0666|IPC_CREAT);
    Regiones * regiones = shmat(shmidStruct, NULL, 0);

    int shmid = shmget(IPC_PRIVATE, cantidad, 0666|IPC_CREAT);
    void * base = shmat(shmid, NULL, 0); 
    char ** archivos = (char**)base;
    char* datos = ((char*)base + nArchivos*sizeof(char*));
    for(int i=0; i<nArchivos; i++){
        archivos[i]= &datos[i*strlen(archNames[i])];
    }

    
    printf("Archivo de texto leído\n");
    for(int i = 0; i < nArchivos; i++){
        for(int j = 0; j < strlen(archNames[i]); j++){
            archivos[i][j]=archNames[i][j];
            printf("%c", archivos[i][j]);
        }
        printf("\n");
    }

    int nhijos = 2;
    int leer = nArchivos/nhijos;
    pid_t root = getpid(); 
    int index =0; 
    for(;index<nhijos; index++){
        if(!fork()){
            break;
        }
    }

    if(root==getpid()){
        regiones->R1=0;
        regiones->R2=0;
        regiones->R3=0;

    }
    else{

        int inicio = index*leer;
        int fin;
        if(index == nhijos-1){
            fin = nArchivos;
        }
        else{

            fin = inicio + leer;
        }

        if(index==0){
            printf("%d\n", fin);

            for(int i = inicio; i<fin; i++){
                //Leemos el archivo e incrementamos las variables en el struct Regiones segun corresponda
                char buffer[1024];
                printf("%s\n", archivos[i]);
                printf("__________________________\n");
                strcpy(buffer, archivos[i]);
            }
        }
        

    }

    return 0;
}
