#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void _kill(pid_t pid, int sig) {
    usleep(1000);
    kill(pid, sig);
}

void handler(int sig) {
    // Manejador 
}
void print_debug_tree(){
  char cmd[50];
   sprintf(cmd, "pstree -lp %d", getpid());
   system(cmd);
}

int main() {
    signal(SIGUSR1, handler);
    pid_t *var = (pid_t *) calloc(4, sizeof(pid_t));
    pid_t root = getpid();
    pid_t child;
    int i;
    int n;
    printf("ingrese la cantidad de veces que se desea repetir: ");
    scanf("%d",&n);
    int j=0;
    for (i = 0; i < 3; i++) {
        if ((var[i] = fork()) == 0) { // H1, H2, H3
            printf("impreso desde el hijo H %d\n", i + 1);
                if (i == 1) {
                    if ((child = fork()) == 0) { // H21
                        printf("impreso desde el hijo H2%d\n", i);
                        break;
                    }
                }
            break;
        }
    }    
// envio de señales
    if (root == getpid()) {
        print_debug_tree();
        while (j<n){
            if(j==0) printf("Padre [%d], \n", getpid());
            _kill(var[i-1], SIGUSR1);
            pause();
            printf("Padre [%d], \n", getpid());
            j++;
        }

        for(int i = 0; i < 3; i++) {
            wait(NULL);
        }
        free(var);
        exit(0);

    } else {
        if (i == 2) {//H3
            while(j<n){
                pause();
                printf("Hijo H3 [%d], \n", getpid());
                _kill(var[i - 1], SIGUSR1);
                j++;
            }
        }
        
        if (i == 1) {// h2
            if(child == 0){//h21
                while(j<n){
                    pause();
                    printf("Hijo H21 [%d], \n", getpid());
                    _kill(getppid(), SIGUSR1);
                    j++;
                }
            }else{//h2
                while(j<n){
                    pause();
                    printf("Hijo H2 [%d], \n", getpid());
                    _kill(child, SIGUSR1);
                    pause();
                    printf("Hijo H2 [%d], \n", getpid());
                    _kill(var[0], SIGUSR1);
                    j++;
                }
            }

        }
        
        if (i == 0) {//h1
            while(j<n){
                pause();
                printf("Hijo H1 [%d],\n ", getpid());
                printf("\n");
                _kill(root, SIGUSR1);
                j++;
            }
        }
    }

    return 0;
}