#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int ready = 0;

void* worker(void* arg) {
    pthread_mutex_lock(&lock);

    while (!ready) {
        pthread_cond_wait(&cond, &lock);
    }

    printf("hilo %ld despertado\n", (long)arg);
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main() {
    pthread_t threads[5];

    for (long i = 0; i < 5; i++)
        pthread_create(&threads[i], NULL, worker, (void*)i);

    sleep(1); // aseguramos que todos estén esperando

    pthread_mutex_lock(&lock);
    ready = 1;
    pthread_cond_broadcast(&cond);   // despierta a los 5 hilos
    pthread_mutex_unlock(&lock);

    for (int i = 0; i < 5; i++)
        pthread_join(threads[i], NULL);
}
