#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_WORKERS 5

// Mutex para la sección crítica
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Barrera
pthread_barrier_t barrier;

// Función que simula el trabajo de cada trabajador
void* worker(void* arg) {
    int id = *(int*)arg;
    
    // Trabajo previo a la barrera (trabajo que puede ser realizado de manera concurrente)
    printf("Worker %d: Doing some work before the barrier...\n", id);
    sleep(1);  // Simulamos que el trabajo toma 1 segundo

    // Después del trabajo, esperamos en la barrera
    pthread_barrier_wait(&barrier);

    // Sección crítica protegida por el mutex
    pthread_mutex_lock(&mutex);
    printf("Worker %d: Entering critical section\n", id);
    sleep(1);  // Simulamos una sección crítica que toma 1 segundo
    printf("Worker %d: Leaving critical section\n", id);
    pthread_mutex_unlock(&mutex);

    // Después de la barrera, los trabajadores pueden continuar con el siguiente trabajo
    printf("Worker %d: Finished work after the barrier\n", id);
    return NULL;
}

int main() {
    pthread_t workers[NUM_WORKERS];
    int worker_ids[NUM_WORKERS];

    // Inicializar la barrera
    pthread_barrier_init(&barrier, NULL, NUM_WORKERS);

    // Crear los hilos para los trabajadores
    for (int i = 0; i < NUM_WORKERS; i++) {
        worker_ids[i] = i + 1;
        if (pthread_create(&workers[i], NULL, worker, &worker_ids[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Esperar a que todos los trabajadores terminen
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(workers[i], NULL);
    }

    // Destruir la barrera
    pthread_barrier_destroy(&barrier);

    return 0;
}
