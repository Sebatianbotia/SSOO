#include<stdlib.h>
#include<stdio.h>
#include<sys/wait.h>
#include<signal.h>
#include<unistd.h>


void __KILL(pid_t pid, int signal){
    usleep(1000);
    kill(pid, signal);
}

void handler(int sig) {}

void print_debug_tree(){
  char cmd[50];
   sprintf(cmd, "pstree -lp %d", getpid());
   system(cmd);
}

void print_message(pid_t hijo, pid_t padre){
    printf("Proceso: %d Padre: %d\n", hijo, padre);   
}

int main(){
    signal(SIGUSR1, handler);
    pid_t root = getpid();
    pid_t *hijos = (pid_t *)malloc(5*sizeof(pid_t));
    pid_t child_g;
    pid_t child_l;
    pid_t child_h;
    pid_t child_j;



    int i = 0;
    int j=0;
    for( ;i < 5; i++){
        if((hijos[i]=fork())==0){
            if (i==1){
                printf("hola desde hijo%d\n", getpid());
                if((child_g=fork())==0){
                    j=1;
                    if((child_l=fork())==0){
                        j=2;
                        break;
                    }
                    break;
                }
            }

            else if(i==3){
                printf("hola desde hijo%d\n", getpid());
                if((child_h=fork())==0){
                    j=3;
                    if((child_j=fork())==0){
                        j=4;
                        break;
                    }
                    break;
                }
            }
            else break;

            break;// esta linea es super importante ya que es la que hace que los procesos que de crean dentro del hijo 1 y 3, rompan.
        }
    }

    if(root== getpid()){
        print_debug_tree();
        __KILL(hijos[i-1], SIGUSR1);
        pause();
        printf("Proceso: %d Padre: papa de todos\n", root);
        for (int j = 0; j < 5; j++){
            wait(NULL);
        }
    }
    else{
    
        pause();//todos los procesos se van a poner en pause
        if (i==1 ){
            if(j==0){
                print_message(getpid(), getppid());
                __KILL(child_g, SIGUSR1);
                pause();
                print_message(getpid(), getppid());
                __KILL(hijos[i-1], SIGUSR1);
            }
            else if (j==1){
                print_message(getpid(), getppid());
                __KILL(child_l, SIGUSR1);
                pause();
                print_message(getpid(), getppid());
                __KILL(getppid(), SIGUSR1);
            }
            else{
                  print_message(getpid(), getppid());
                __KILL(getppid(), SIGUSR1);
            }

        }
        else if (i==3){
            if(j==0){
                print_message(getpid(), getppid());
                __KILL(child_h, SIGUSR1);
                pause();
                print_message(getpid(), getppid());
                __KILL(hijos[i-1], SIGUSR1);
            }
            else if (j==3){
                print_message(getpid(), getppid());
                __KILL(child_j, SIGUSR1);
                pause();
                print_message(getpid(), getppid());
                __KILL(getppid(), SIGUSR1);
            }
            else{
                  print_message(getpid(), getppid());
                __KILL(getppid(), SIGUSR1);
            }
        }
        else{
            print_message(getpid(), getppid());
            __KILL(hijos[i-1], SIGUSR1);

        }
    }


    return 0;
}