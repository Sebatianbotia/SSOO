#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<signal.h>
int** read_matrix(const char* fname, int* f, int* c){

    FILE* ff = fopen(fname, "r");
    if(!ff){
        perror("File");
        abort();
    }

    int filas, columnas;
    fscanf(ff, "%d, %d", &filas, &columnas);
    *f = filas;
    *c = columnas;

    int** m = (int**)malloc(sizeof(int*)*filas);
    for(int i = 0; i < filas; i++){
        m[i] = (int*)malloc(sizeof(int)*columnas);
    }

    for(int i = 0; i < filas; i++){
        for(int j = 0; j < columnas; j++){
            fscanf(ff, "%d", &m[i][j]);
        }
    }

    fclose(ff);
    return m;
}

void handler(int s){}

int main(int argc, char** argv){

    if(argc < 2){
        perror("argcs");
        return EXIT_FAILURE;
    }

    signal(SIGUSR1, handler);
    
    int f, c;
    (void*)read_matrix(argv[1], &f, &c);

    int shmid = shmget(IPC_PRIVATE, f*c*sizeof(int)+f*sizeof(int*), 0666 | IPC_CREAT);

    void* base_ptr = (void*)shmat(shmid, NULL, 0);
    int** matriz = (int**)base_ptr;
    int* datos = (int*)((char*)base_ptr + f*sizeof(int*));
    for(int i = 0; i < f; i++){
        matriz[i] = &datos[i*c];//direccion de datos en posicion n
    }

    pid_t child;
    if((child = fork())){ //padre

        int **mat_aux = read_matrix(argv[1], &f, &c);

        for(int i = 0; i < f; i++){
            for(int j = 0; j < c; j++){
                matriz[i][j] = mat_aux[i][j];
            }
        }
        usleep(60000);
        kill(child, SIGUSR1);
        //pause();
        wait(NULL);
        printf("------------PADRE---------------\n");
        for(int i = 0; i < f; i++){
            for(int j = 0; j < c; j++){
                printf("%d ", matriz[i][j]);
            }
            printf("\n");
        }
        shmdt(base_ptr);
        shmctl(shmid, IPC_RMID, NULL);

    }else{//hijo
        pause();
        printf("------------HIJO---------------\n");
        for(int i = 0; i < f; i++){
            for(int j = 0; j < c; j++){
                printf("%d ", matriz[i][j]);
                matriz[i][j]+= 4;
            }
            printf("\n");
        }
        //kill(getppid(), SIGUSR1);
        shmdt(base_ptr);
    }

    return EXIT_SUCCESS;
}