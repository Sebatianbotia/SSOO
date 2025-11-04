#include<stdlib.h>
#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<signal.h>


#define PROP 1
#define CONS 2
#define REV 3


void handler(int signal){}

void __KILL(pid_t pid, int signal){
    usleep(100000);
    kill(pid, signal);
}

//leer el archivo en formato dado
int ** read_file( const char * fName, int *f, int * c, int * horas , int * nHijos){
    FILE * fd = fopen(fName,  "r");
    if(!fd){
        perror("file");
        abort();

    }

    fscanf(fd, "%d", horas);
    fscanf(fd, "%d", nHijos);
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

int search_neighbors(int** A, int f, int c, int fila, int columna, int target){
    int i_fila = fila - 1;
    int e_fila = fila + 1;
    int i_columna = columna - 1;
    int e_columna  = columna + 1;

    int found = 0;

    //found = (columna-1 >= 0 && A[fila][columna-1] == target) ? found+1 : 0; Asi para arriba, abajo, izq y der

    for(int i = i_columna; i <= e_columna; i++){
        for(int j = i_fila; j <= e_fila; j++){
            if(j >= 0 && i >=0 && i < f && j < c && i != fila && j != columna){
                if(A[i][j] == target) found++;
            }
        }
    }

    return found;
}

void propagation(int ** principal, int ** secondary, int filas, int columnas, int inicio, int final ){

    int quemandose=0;

    for(int i = inicio; i<final; i++){
        for(int j=0; j< columnas; j++){
            
            quemandose = search_neighbors(principal, filas, columnas, i , j, 1);
            if(quemandose >= 2) secondary [i][j] = 1;            
        }
    }

}

void consumo (int ** principal, int ** secondary, int filas, int columnas, int inicio, int final){
    int quemandose;
        for(int i = inicio; i<final; i++){
        for(int j=0; j< columnas; j++){
            if(principal[i][j]==1){
                secondary[i][j]=-1;
            }
        }
    }
}

typedef struct Data{
    int fase;
    int final;
    int bandera;
} Data;

int main(){
    signal(SIGUSR1, handler);
    int filas, columnas, horas, nHijos;
    int shmidStr = shmget(IPC_PRIVATE, sizeof(Data), 0666|IPC_CREAT);
    Data *data = shmat(shmidStr, NULL, 0);
    //printf("hola\n");

    data->bandera=0;
    data->fase = PROP;
    data->final=0;

    int ** mat = read_file("matProp.txt",&filas, &columnas, &horas, &nHijos);
    //log_matrix(mat, filas, columnas);
    //segmento de memoria para matriz principal
    int shmidPR = shmget(IPC_PRIVATE,sizeof(int)*filas*columnas + sizeof(int*)*filas, 0666| IPC_CREAT);
    void * base = shmat(shmidPR, NULL, 0);
    int ** principal = (int**)base;
    int * datos = (int*)((char*)base + sizeof(int*)*filas);
    for(int i = 0; i<filas; i++){
        principal[i] = &datos[i*columnas];
    }


    //segmento de memoria para matriz secundaria
    int shmidSC = shmget(IPC_PRIVATE,sizeof(int)*filas*columnas + sizeof(int*)*filas, 0666| IPC_CREAT);
    void * baseSC = shmat(shmidSC, NULL, 0);
    int ** secondary = (int**)baseSC;
    int * datosSC = (int*)((char*)baseSC + sizeof(int*)*filas);
    for(int i = 0; i<filas; i++){
        secondary[i] = &datosSC[i*columnas];
    }

    move_mat(mat, principal, filas, columnas);
    move_mat(mat, secondary, filas, columnas);
    printf("\n");
    //log_matrix(principal, filas, columnas);
    printf("\n");
    //log_matrix(secondary, filas, columnas);
    
    // en data está el struct
    // en principal la matriz principal 
    // en secondary la matriz secundaria que es la que van a modificar los hijos
    
    
    pid_t* hijos = (pid_t *)malloc(sizeof(pid_t)*nHijos); //vactor de hijos
    
    
    if(!hijos){
        perror("memory");
        return 1;
    }
    
    pid_t root = getpid();
    
    int index=0;
    
    for(; index < nHijos; index++){
        if(!(hijos[index]=fork())){
            break;
        }
    }
    
    int horaActual=0;
    
    if(root == getpid()){
        while(horaActual < horas){
            data->fase=PROP;
            for(int j = 0 ; j<nHijos;j++){
                __KILL(hijos[j], SIGUSR1);
            }
            pause(); //espera a los que los hijos terminen la fase de propagación
           // printf("aaaaaa\n");
            printf("fase 1 terminada, hora: %d\n", horaActual);
            data->fase=CONS;
            data->bandera=0;

            for(int j =0 ; j<nHijos;j++){
            printf("actualizando cambios...");
                usleep(10000);
                __KILL(hijos[j], SIGUSR1);
            }
            pause(); 
            printf("fase 1 terminada, hora: %d\n", horaActual);

            data->fase=REV;
            data->bandera=0;
            printf("matriz resultante despues de hora %d:\n", horaActual);
            log_matrix(secondary, filas, columnas);
            printf("actualizando cambios...\n");
            sleep(2);
            move_mat(secondary, principal, filas, columnas);
            printf("cambios actualizados\n");
            sleep(1);
            
            horaActual++;
        }
        printf("fin de la simulacion.");
        data->final=1;
        for(int j =0 ; j<nHijos;j++){
                __KILL(hijos[j], SIGUSR1);
        }
        for(int h=0;h<nHijos;h++){
            wait(NULL);
        }


    }
    else{
        int delta = filas/nHijos;
        int inicio = index*delta;
        int fin = (index == nHijos-1) ? filas : inicio + delta;
        while (1){
            pause();
            
            if(data->final==1)break;
            if(data->fase==PROP){
                propagation(principal, secondary, filas, columnas, inicio, fin);
                //printf("hijo %d\n", index);
                
            }
            else if (data->fase == CONS){
                consumo(principal, secondary, filas, columnas, inicio, fin);
            }
            
            data->bandera++;
            
            //printf("aaaaaa data: %d\n", data->bandera);
            if(data->bandera== nHijos){
                printf("envindo señal al padre...\n");
                usleep(1000);
                kill(root, SIGUSR1);
            }

        }
    }

    return 0;

}