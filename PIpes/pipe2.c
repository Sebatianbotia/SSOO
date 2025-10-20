#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

void print_debug_tree(){
  char cmd[50];
   sprintf(cmd, "pstree -lp %d", getpid());
   system(cmd);
}


int main(){
    int nHijos;
    char buffer[100];
    printf("ingrese el numero de hijos: ");
    scanf("%d", &nHijos);
    int nPipes = nHijos+1;
    pid_t root = getpid();
    int (*pipes)[2];
    pipes = (int(*)[])malloc(nPipes*sizeof(int[2]));
    
    for(int j=0; j<nPipes;j++){
        if(pipe(pipes[j])==-1){
            perror("error al crear la tuberia");
            exit(1);
        }
    }
    int i = 0;
    for(; i < nHijos;i++){
        if (!fork()) break;
    }

    if(root==getpid()){//padre
        char msg[100];
       

        printf("ingrese en mensaje: ");
        scanf("%s", msg);

        close(pipes[0][0]);
        close(pipes[nPipes-1][1]);
        for(int i = 1; i < nPipes-1;i++){
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
        print_debug_tree();
        write(pipes[0][1],&msg, strlen(msg)+1);
        read(pipes[nPipes-1][0], &buffer, sizeof(buffer));
        printf("Pid:  %d, message: %s\n", root, buffer);
        close(pipes[0][1]);
        close(pipes[nPipes-1][0]);
        
        for(int j=0;j<nHijos;j++){
            wait(NULL);
        }
        
    }
    else{
        //hijos
        for (int j = 0; j < nHijos; j++){// aca cerramos las tuberias que no usará cada hijo,
                                         // cada uno lee del tubo i y escribe en el i+1, excepto el 3, pero tambien se cumple
            if(j==i) {
                close(pipes[j][1]);
                //close(pipes[j+1][0]);
            } else if(j == i+1){
                close(pipes[j][0]);
            }
            else {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
        }
        sleep(1);
        read(pipes[i][0], &buffer, sizeof(buffer));
        printf("Pid:  %d, message: %s\n", getpid(), buffer);
        write(pipes[i+1][1], &buffer, strlen(buffer)+1);
        close(pipes[i][0]);
        close(pipes[i+1][1]);

    }

    free(pipes);
    return 0; 
}