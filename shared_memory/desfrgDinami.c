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
void log_matrixf(float** M, int f, int c){
    for(int i = 0; i < f; i++){
        for(int j = 0; j < c; j++){
            printf("%f ", M[i][j]);
        }
        printf("\n");
    }
}
void __KILL(pid_t pid, int signal){
    usleep(100000);
    kill(pid, signal);
}

void handler(int signal){}

void move_mat(int ** origin, int** destino, int f, int c){
    for(int i =0; i<f;i++){
        for(int j=0;j<c;j++){
            destino[i][j]=origin[i][j];
        }
    }
}

float fragmentacion(int **matO, int ** matR, int ** matT, int i, int j ){
    float dato=0;
    dato = 100 * (matO[i][j]==1) * (9-matR[i][j])*(matT[i][j]+1)/90;
    return dato;
}

float recoleccion ( int **matO, int ** matR, int ** matT, int i, int j){
    float dato=0;
    dato = 50*(matO[i][j]==2)*(1+matT[i][j])*(1+(9-matR[i][j]))/100;
    return dato;
}

int decision ( float **matP, float ** matF, int i, int j){
    int dato;
    if(matF[i][j]>0.7 && matP[i][j]>0.8){
        dato = 2;
    }
    else if (matF[i][j]>0.5 && matP[i][j]>0.6){
        dato =1;
    }
    else{
        dato = 0;
    }
    //printf("dato-> %d\n", dato);
    return dato;
}

int main(){

    signal(SIGUSR1,handler);
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

    //Creacion de segmento de memoria compartida para matriz F (fragmentacion)
    int shmid_F = shmget(IPC_PRIVATE, sizeof(float)*filas*columnas+sizeof(float*)*filas, 0666|IPC_CREAT);
    void * baseF = shmat(shmid_F, NULL, 0);
    float ** F = (float**)baseF;
    float * datos_F = (float*)((char*)baseF + filas*sizeof(float*));
    for(int i=0; i<filas; i++){
        F[i]= &datos_F[i*columnas];
    }

    //Creacion de segmento de meoria compartida para matriz P (prioridad)
    int shmid_P = shmget(IPC_PRIVATE, sizeof(float)*filas*columnas+sizeof(float*)*filas, 0666|IPC_CREAT);
    void * baseP = shmat(shmid_P, NULL, 0);
    float ** P = (float**)baseP;
    float * datos_P = (float*)((char*)baseP + filas*sizeof(float*));
    for(int i=0; i<filas; i++){
        P[i]= &datos_P[i*columnas];
    }

    //creacion de segment6o matriz Decisiones
    int shmid_D = shmget(IPC_PRIVATE, sizeof(int)*filas*columnas+sizeof(int*)*filas, 0666|IPC_CREAT);
    void * baseD = shmat(shmid_D, NULL, 0);
    int ** D = (int**)baseD;
    int * datos_D = (int*)((char*)baseD + filas*sizeof(int*));
    for(int i=0; i<filas; i++){
        D[i]= &datos_D[i*columnas];
    }
    
    //CREACION DEL SEGMENTO DE MEMORIA COMPARTIDA PATA MATRIZ T
    int shmid_T = shmget(IPC_PRIVATE, sizeof(int)*filas*columnas+sizeof(int*)*filas, 0666|IPC_CREAT);
    void * baseT = shmat(shmid_T, NULL, 0);
    int ** T_shm = (int**)baseT;
    int * datos_T = (int*)((char*)baseT + filas*sizeof(int*));
    for(int i=0; i<filas; i++){
        T_shm[i]= &datos_T[i*columnas];
    }

    //End
    int shmid_end = shmget(IPC_PRIVATE, sizeof(int), 0666|IPC_CREAT);
    int * end = shmat(shmid_end, NULL, 0);

    //montamos matriz t a memoria compartida
    move_mat(T, T_shm, filas, columnas);
    pid_t* hijos = (pid_t *)malloc(sizeof(pid_t)*2);
     if(!hijos){
        perror("memory");
        return 1;
    }
    pid_t root = getpid();
    int index = 0;
    for(; index<2; index++){
        if(!(hijos[index]=fork())){
            break;
        }
    }
    
    if(root==getpid()){ //padre
        int cicloActual = 0;
        *end = 0;
        while(cicloActual<8){
             __KILL(hijos[0], SIGUSR1);
            pause();
            __KILL(hijos[1], SIGUSR1);
            pause();
            for(int i = 0; i<filas; i++){
                    for(int j=0;j<columnas;j++){
                        D[i][j] = decision(P, F, i, j);
                        //printf("dato-> %d\n", D[i][j]);
                        if(T_shm[i][j] < 9){
                            T_shm[i][j]++;
                        }
                    }
            }
            printf("matriz de desicion ciclo %d: \n", cicloActual+1);
            log_matrix(D, filas, columnas);
            printf("-------------------------------------------\n");
            
            cicloActual++;
        }

        *end=1;
        __KILL(hijos[0], SIGUSR1);
        __KILL(hijos[1], SIGUSR1);
       for(int i =0; i<2; i++){
        wait(NULL);
       }


    }
    else{
        if(index==0){//hijo 1
            while(1){
                pause();
                if(*end ==1) break;
                for(int i =0; i<filas; i++){
                    for(int j=0;j<columnas;j++){
                        F[i][j] = fragmentacion(O,R,T_shm,i,j);
                    }
                }
                log_matrixf(F, filas, columnas);
                __KILL(getppid(), SIGUSR1);
            }
        }
        else{
             while(1){
                pause();
                if(*end ==1) break;
                for(int i =0; i<filas; i++){
                    for(int j=0;j<columnas;j++){
                        P[i][j] = recoleccion(O,R,T_shm,i,j);
                    }
                }
                printf("----------------------------\n");
                log_matrixf(P, filas, columnas);
                __KILL(getppid(), SIGUSR1);
            }
        }
    }
    
    
    
    
    return 0;
}