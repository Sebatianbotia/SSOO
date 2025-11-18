#include<stdlib.h>
#include<pthread.h>
#include<stdio.h>

int counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * worker(void* arg){
    printf("hola\n");
    for(int i = 0; i < 1000;i++){
        pthread_mutex_lock(&lock);//se controla el acceso a la variabe compartida
        counter++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main(){

    pthread_t h1, h2;
    pthread_create(&h1, NULL, worker, NULL);
    pthread_create(&h2, NULL, worker, NULL);

    pthread_join(h1, NULL);
    pthread_join(h2, NULL);

    printf("counter: %d\n", counter);

    return 0;
}