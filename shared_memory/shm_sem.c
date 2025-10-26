#include<stdio.h>
#include<stdlib.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/ipc.h>

int main(){

    int id = shmget(IPC_PRIVATE, sizeof(int), 0666 | IPC_CREAT);
    int *p = shmat(id, NULL, 0);

    pid_t root = getpid();

    int i = 0;
    *p=-1;
    int nHijos=3;
    for(;i<nHijos;i++){
        if(!fork()){
            break;;
        }
    }
    if(root==getpid()){
        printf("activando cadena de hola como estas\n");
        *p=i-1;
        for(int j=1;j<nHijos;j++){
            wait(NULL);
        }
    }else{  
        while(*p!=i){

        }
        printf("hola como estas pid: %d\n", getpid());
        *p=i-1;

    }


    


        
    
    

    return 0;
}