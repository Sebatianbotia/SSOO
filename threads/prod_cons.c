#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

//Productor - consumidor clasico en C usando mutex y variables de condicion
#define BUFFER_SIZE 5
int buffer[BUFFER_SIZE];
int in = 0; // indice de escritura
int out = 0; //indice de lectura
int count = 0; //cuantos elementoS hay

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

void * producer(void * arg){
    for(int i =0 ;i<20;i++){
        pthread_mutex_lock(&lock);
        while(count == BUFFER_SIZE){
            pthread_cond_wait(&not_full, &lock);
        }

        buffer[in]= i;
        in = (in + 1) % BUFFER_SIZE;
        count ++;

        printf("productor produce %d (count 0 %d)\n", i, count);
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&lock);

        usleep(100000);
    }
    return NULL;
}

void * consumer(void * arg){
    for(int i =0; i< 20; i++){
        pthread_mutex_lock(&lock);
        while(count == 0){
            pthread_cond_wait(&not_empty, &lock);
        }
        int item = buffer[out];
        out = (out+1)%BUFFER_SIZE;
        count --;

        printf("Consumidor consume %d (count = %d)\n", item, count);
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&lock);
        usleep(150000);
    }
    return NULL;
}

int main(){
    pthread_t productor, consumidor;
    pthread_create(&productor, NULL, producer, NULL);
    pthread_create(&consumidor, NULL, consumer, NULL);

    pthread_join(productor,NULL);
    pthread_join(consumidor, NULL);

    return 0;
}