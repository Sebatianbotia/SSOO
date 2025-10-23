#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>
#include<stdio.h>

void __KILL(pid_t pid, int signal){
    usleep(1000);
    kill(pid, signal);
}

void handler( int signal ){}

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
    pid_t *hijos = (pid_t*)malloc(5 * sizeof(pid_t));
    int i = 0;
    pid_t child;
    int j=0;
    for(;i<5;i++){
        if ((hijos[i]=fork())==0){
            if(i%2==0){
                if((child = fork())==0){
                    j+=1;
                    break;
                }
                break;
            }
            break;
        }
    }

    if(root==getpid()){
        print_debug_tree();
        __KILL(hijos[i-1], SIGUSR1);
        pause();
        printf("el papa de los pollitos %d\n", root);
        for(int h =0; h<5;h++){
            wait(NULL);
        }
    }
    else{
        pause();
        if(i%2==0){
            if(i!=0 && j==0){
                print_message(getpid(), getppid());
                __KILL(child, SIGUSR1);
                pause();
                print_message(getpid(), getppid());
                __KILL(hijos[i-1], SIGUSR1);
            }
            
            else if(j==1){
                print_message(getpid(), getppid());
                __KILL(getppid(), SIGUSR1);
            }

            else{
                print_message(getpid(), getppid());
                __KILL(child, SIGUSR1);
                pause();
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

