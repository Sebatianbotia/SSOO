#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<signal.h>
#include<string.h>

#define MAX_ARCHIVOS 50
#define MAX_LENGTH 100

typedef struct {
    int n;
    char archivos[MAX_ARCHIVOS][MAX_LENGTH];

} Archi;

int main(int argc, char **argv){

    int shmid = shmget(IPC_PRIVATE, sizeof(Archi), 0666|IPC_CREAT);
    if(!shmid){
        perror("memoria");
        return -1;
    }

    Archi * archi = shmat(shmid, NULL, 0);

    FILE * fd = fopen("fnt.txt", "r");

   
    int nHijos = atoi(argv[1]);
    
    pid_t root = getpid();
    int index = 0;
    for(; index<nHijos; index++){
        if(!fork()){
            break;
        }
    }
    
    if(root==getpid()){
        int cantidad;
        
        fscanf(fd, "%d", &cantidad);
        
        archi->n=cantidad;
        //printf("%d\n", archi->n);
        for(int i = 0; i<cantidad;i++){
            fscanf(fd, "%s", archi->archivos[i]);
        }
        
        for(int i=0;i<nHijos;i++){
            wait(NULL);
        }
        
        
    }
    else{
        sleep(1);
        int leer = (archi->n)/nHijos;
        int inicio = index*leer;
        int fin;
        if(index == nHijos-1){
            fin = archi->n;
        }
        else{
            fin = inicio+leer;
        }

        for(int i = inicio;i<fin;i++){
            //aqui va la logica de leer, etc.
        }


    }





    return 0;
}