#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>

void handler(int signal){}

void __KILL(pid_t pid, int sig){
    usleep(1000);
    kill(pid, sig);
}

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
    int hijos[3];
    pid_t root = getpid();
    pid_t child;

    int i=0;
    int j=0;
    for(; i<3;i++){
        if((hijos[i]=fork())==0){
            if((child=fork())==0){

                j+=1;
                if(i==1){

                    if((child=fork())==0){
                        j+=1;

                        if((child=fork())==0){
                            j+=1;
                            break;
                        }
                        
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        }
    }

    if(root==getpid()){

        print_debug_tree();
        __KILL(hijos[i-1],SIGUSR1);
        pause();
        printf("el papa de los pollitos %d\n", getpid());
        for(int h=0;h<3;h++){
            wait(NULL);
        }
    }
    else{
        pause();
        if(i==1){
            if(j==0||j==1||j==2){
                print_message(getpid(), getppid());
                __KILL(child, SIGUSR1);
                pause();
                print_message(getpid(), getppid());
                if(j==0){
                    __KILL(hijos[i-1], SIGUSR1);
                }
                else{
                    __KILL(getppid(),SIGUSR1);
                }
            }
            else {
                print_message(getpid(), getppid());
                __KILL(getppid(), SIGUSR1);
            }
        }else if(i==0){
            if(j==0){
                print_message(getpid(), getppid());
                __KILL(child, SIGUSR1);
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

            if(j==0){
                print_message(getpid(), getppid());
                __KILL(child, SIGUSR1);
                pause();
                print_message(getpid(), getppid());
                __KILL(hijos[i-1], SIGUSR1);

            }
            else{

                print_message(getpid(), getppid());
                __KILL(getppid(), SIGUSR1);
            }
        }
    }




    return 0;
}