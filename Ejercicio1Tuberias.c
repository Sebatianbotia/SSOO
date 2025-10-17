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
    int nHijos = 4;
    int (*pipes)[2];
    pipes = (int (*)[2])malloc(nHijos*sizeof(int[2]));
    char buffer[200];
    //en el int(*)[2]: estamos diciendo haciendo el 
    //cast a un puntero de un array de dos enteros
    //en el malloc en sizeof(int[2]): estamos haciendo el calculo para un vector de dos enteros.
    pid_t root = getpid();

    for(int j=0;j<nHijos;j++){ //creacion de los pipes
        if(pipe(pipes[j])==-1){
            perror("error al crear el pipe");
            exit(1);
        }
    }
    int i = 0;
    for(; i<2; i++){
        if(!fork()){ //creamos los dos primeros hijos 0,1
            break;
        }
    }
    if(root == getpid()){//padre
        //aca hay que cerrar las ttuberias que no va a usar el padre

        close(pipes[0][0]);
        for(int i = 1; i < nHijos; i++){
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        printf ("Soy el padre \n");

        print_debug_tree();

        char msg[100];
        //int msg = 5; 
        //write(pipes[0][1], &msg , strlen(msg)+1);
        while(1) {
            printf("Ingrese mensaje (o 'fin' para terminar): ");
            scanf("%s", msg);
            if(strcmp(msg, "fin") == 0) {
                printf("Finalizando...\n");
                break;
            }
            
            write(pipes[0][1], msg, strlen(msg) + 1);
            usleep(1000);

        }
        close(pipes[0][1]);
        for( int i =0; i<2; i++){
            wait(NULL);
        }
    }
    else {
        if(i==0){

            if(!fork()) i = 2; // creacion del hijo 2
        }
        else{
            if(!fork()) i = 3;
        }
        sleep(1);

        #define msg(X, Y) printf("hijo: %d -> cerrando tuberia: %d\n", X, Y);
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
        
        //read(pipes[i][0], buffer, sizeof(buffer));
        //printf("hijo: %d -> valor: %s\n", i, buffer);
        //close(pipes[i][0]);


        //if(i!= 3){
         //   write(pipes[i+1][1], buffer, strlen(buffer)+1);   
         //   close(pipes[i+1][1]);
        //}
        while(read(pipes[i][0], buffer, sizeof(buffer)) > 0){
            printf("hijo: %d -> valor: %s\n", i, buffer);
            
            // Si no es el último, escribir al siguiente
            if(i != 3){
                write(pipes[i+1][1], buffer, strlen(buffer) + 1);
            }
        }
        close(pipes[i][0]); // Cerrar después de leer
        
        if(i != 3){
            close(pipes[i+1][1]); // Cerrar después de escribir
        }
    }
    
    free(pipes);
    return 0;

}