#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void handler(int sig) {}

int main() {

    signal(SIGUSR1, handler);
    
    pid_t root = getpid();
    pid_t h1, h2, h3, h21;
    
    
    if ((h1 = fork()) == 0) {
        pause();
        printf("hijo1 [%d]\n", getpid());
        kill(root, SIGUSR1);
        exit(0);
    }
    
    if ((h2 = fork()) == 0) {
        
        if ((h21 = fork()) == 0) {
            pause();
            printf("hijo21 [%d]\n", getpid());
            kill(getppid(), SIGUSR1);
            exit(0);
        }
        pause();
        printf("hijo2 [%d]\n", getpid());
        kill(h21, SIGUSR1);
        pause();
        printf("hijo2 [%d]\n", getpid());
        kill(h1, SIGUSR1);
        exit(0);
    }

    if ((h3 = fork()) == 0) {
        pause();
        printf("hijo3 [%d]\n", getpid());
        kill(h2, SIGUSR1);
        exit(0);
    }
    
    sleep(1);
    printf("Padre [%d]\n", root);
    kill(h3, SIGUSR1);
    pause();
    printf("Padre [%d]\n", root);

    for(int i = 0; i < 3; i++) {
        wait(NULL);
    }
    
    return 0;
}