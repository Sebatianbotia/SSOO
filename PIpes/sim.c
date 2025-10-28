#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<signal.h>



typedef struct Interruption {
    int n;
    int interruptionId;
} Interruption;

void handler(int signal){}

int main(){
    signal(SIGUSR1, handler);
    pid_t root = getpid();
    int nhijos =3;
    int memId = shmget(IPC_PRIVATE, sizeof(int)*2, 0666|IPC_CREAT);

    int * memory = shmat(memId, NULL, 0);
    int i=0;
    int(*fd)[2]=malloc(sizeof(int(*)[2])*nhijos);

    pid_t *hijos = (pid_t*)malloc(nhijos*sizeof(pid_t));


    for(int j=0;j<nhijos;j++){

        if(pipe(fd[j])==-1){
            perror("error");
            exit(1);
        }

    }
    for(;i<nhijos;i++){
        if((hijos[i]=fork())==0) break;
    }
    
    
    if(root == getpid()){
        int  interruptionVec[6] = {1,2,3,4,5,6};
        for(int j = 0; j<nhijos;j++){
            close(fd[j][0]);
        }
        *memory =-1;
        *(memory+1)=-1;
        int nHijo;
        int interruption;
        
        char message[30] = "interrupcion ";

        while (*memory==-1 && (*(memory+1)==-1)){  
            printf("ingrese el numero del hijo: ");
            scanf("%d", &nHijo);
            printf("ingrese el numero de la interrupción: ");
            scanf("%d", &interruption);
            *memory = nHijo-1;
            *(memory+1) = interruption;
            usleep(1000);
            kill(hijos[nHijo-1], SIGUSR1);
            write(fd[*memory][1], message, strlen(message)+1);
            sleep(1);
            printf("esperando interrupciones...\n");
            sleep(3);
            printf("interrupcion detectada\n");
            sleep(2);
            *memory =-1;
            *(memory+1)=-1;

         }

        // printf("%d\n", *memory);
        // printf("%d\n", *(memory+1));


        // 


        // close(fd[*memory][1]);

        for(int h =0; h<nhijos;h++){
            wait(NULL);
        }
    }
    else {
        for(int j = 0; j<nhijos;j++){
                    close(fd[j][1]);
                if(i!=j) close(fd[j][0]);
        }
        char buffer[100];

        
        while(1){
            pause();
            printf("desperte %d\n", getpid());
            read(fd[i][0], buffer, sizeof(buffer));
            printf("%s %d resuelta por mi papa %d\n", buffer,*(memory+1), getppid());

        }
        // if(i==0){
        //     sleep(1);
        //     *memory = i;
        //     *(memory+1) = 3;
        //     int bl = read(fd[0][0], buffer, sizeof(buffer));
        //     printf("%d \n ", bl);
        //     printf("%s pid %d\n", buffer, getpid());
        // }

    }



    return 0;
}