#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/stat.h>

#define NC 3
#define OPEN 0x1
#define CLOSE 0x0
#define EXIT -10

typedef struct Orden {
    int compuerta;
    int accion;
} Orden;

int main(){

    int shmid = shmget(IPC_PRIVATE, sizeof(Orden), 0666 | IPC_CREAT);

    Orden* orden = (Orden*)shmat(shmid, NULL, 0);
    orden->compuerta = -1;

    pid_t padre = getpid();

    int i = 0;
    for(; i < NC; i++){
        if(!fork()) break;
    }

    if(padre == getpid()){
        int comp;
        int acc;
        do{
            printf(">>> ");
            scanf("%d %d", &comp, &acc);
            orden->compuerta = comp-1;
            orden->accion = acc;
        }while(acc != EXIT);

        for(int i = 0; i < NC; i++) wait(NULL);

    }else{
        printf("Hijo: %d\n", i);
        int estado = 0;
        while(orden->compuerta != EXIT){

            while(orden->compuerta != i || orden->accion != EXIT){
            }
            
            if(orden->accion == 1){
                if(estado == 1){
                    printf("La compuerta %d ya esta abierta\n", i+1);
                }else{
                    printf("Abriendo compuerta -> %d ...\n", i+1);
                    sleep(1);
                    printf("Compuerta -> %d abierta\n", i+1);
                    estado = 1;
                }
            } else if (orden->accion == 0){
                if(estado == 0){
                    printf("La compuerta %d ya esta cerrada\n", i+1);
                }else{
                    printf("Cerrando compuerta -> %d ...\n", i+1);
                    sleep(1);
                    printf("Compuerta -> %d cerrada\n", i+1);
                    estado = 0;
                }
            }
            orden->compuerta = -1;
        }

    }
    return EXIT_SUCCESS;
}