#include <stdio.h>
#include<stdio.h>>
#include<unistd.h>
#include<sys/wait.h>

void print_debug_tree(){
  char cmd[50];
   sprintf(cmd, "pstree -lp %d", getpid());
   system(cmd);
}


int main(){
    int nHijos=0;
    printf("Ingrese mensaje (o 'fin' para terminar): ");
    scanf("%d", nHijos);
    int nPipes = nHijos+1;
    pid_t root = getpid();
    int (*pipes)[2];
    pipes = (int(*)[])malloc(nPipes*sizeof(int[2]));
    
    for(int j=0; j<nPipes;j++){
        if(pipe(pipes[j])==-1){
            perror("error al crear la tuberia");
            exix(1);
        }
    }
    int i = 0;
    for(; i < nHijos;i++){
        if (!fork()) break;
    }

    if(root==getpid()){
        printf("estoy en el padre");
        char msg[100];
        printf("ingrese en mensaje: ");
        scanf("%s", msg);
        close(pipes[0][0]);
        close(pipes[nPipes][1]);
        for(int i = 1; i < nPipes;i++){
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
        
    }else{
        //hijos
        for (int j = 0; j < nHijos; j++){// aca cerramos las tuberias que no usará cada hijo,
                                         // cada uno lee del tubo i y escribe en el i+1, excepto el 3, pero tambien se cumple
            if(j==i) {
                //msg(i,j)
                close(pipes[j][1]);
                //close(pipes[j+1][0]);
            } else if(j == i+1){
                //msg(i,j)
                close(pipes[j][0]);
            }
            else {
                //msg(i,j)
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
        }
    }


    return 0; 
}