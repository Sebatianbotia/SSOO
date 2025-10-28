#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>

void print_debug_tree(){
    char cmd[50];
    sprintf(cmd, "pstree -lp %d", getpid());
    system(cmd);
}

int main(){
    char buffer2[50];
    char buffer[50];
    int nHijos;
    printf("ingrese el numero de hijos: ");
    scanf("%d", &nHijos);
    int nPipes = nHijos*2;
    int (*pipes)[2];
    pipes = (int(*)[2])malloc(nPipes*sizeof(int[2]));
    pid_t root = getpid();
    pid_t *hijos=(pid_t*)malloc(nHijos*sizeof(pid_t));
    
    for(int j=0; j<nPipes;j++){
        if(pipe(pipes[j])==-1){
            perror("error al crear la tuberia");
            exit(1);
        }
    }
    
    int i=0;
    for(;i<nHijos;i++){
        if((hijos[i]=fork())==0){
            break;
        }
    }
    
    if(getpid()==root){
        print_debug_tree();
        for (int j = 1; j < nPipes-1; j++){
            close(pipes[j][0]);
            close(pipes[j][1]);
        }
        close(pipes[0][0]); 
        close(pipes[nPipes-1][1]); 
        
        char message[50];

        printf("ingrese en mensaje: ");
        scanf("%s", message);
        printf("message: %s  padre: %d\n", message, getpid());
        write(pipes[0][1], message, strlen(message)+1);
        close(pipes[0][1]);
        
        read(pipes[nPipes-1][0], buffer, sizeof(buffer));
        printf("message: %s  padre: %d\n", buffer, getpid());
        close(pipes[nPipes-1][0]);
        
        for(int j =0; j<nHijos; j++){
            wait(NULL);
        }
        
        free(pipes);
        free(hijos);
    }
    else{
        sleep(1);
        if(i==nHijos-1){
            for(int j = 0; j < nPipes; j++){
                if(j != i && j != i+1){
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
            }
            close(pipes[i][1]);     
            close(pipes[i+1][0]);    
            
            read(pipes[i][0], buffer, sizeof(buffer));
            printf("message: %s  pid %d ultimo hijo \n", buffer, getpid());
            printf("iniciando retorno de informacion...\n");
            sleep(3);
            close(pipes[i][0]);
            
            write(pipes[i+1][1], buffer, strlen(buffer)+1);
            close(pipes[i+1][1]);
        }
        else{
            int leerVuelta = nPipes - 2 - i;
            int escribirVuelta = leerVuelta + 1;
            
            for(int j = 0; j < nPipes; j++){
                if(j != i+1 && j != i && j != leerVuelta && j != escribirVuelta){
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
            }
            close(pipes[i][1]);                 
            close(pipes[i+1][0]);                
            close(pipes[leerVuelta][1]);      
            close(pipes[escribirVuelta][0]); 

            read(pipes[i][0], buffer, sizeof(buffer));
            printf("message: %s  pid %d\n", buffer, getpid());
            close(pipes[i][0]);
            
            write(pipes[i+1][1], buffer, strlen(buffer)+1);
            close(pipes[i+1][1]);
            
            read(pipes[leerVuelta][0], buffer2, sizeof(buffer2));
            printf("message: %s  pid %d\n", buffer2, getpid());
            close(pipes[leerVuelta][0]);
            
            write(pipes[escribirVuelta][1], buffer2, strlen(buffer2)+1);
            close(pipes[escribirVuelta][1]);
        } 
    }
    return 0;
}