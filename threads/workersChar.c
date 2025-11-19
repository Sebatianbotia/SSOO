#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define NUM_WORKERS 4
#define MAX_STRING_LENGTH 100

char* strings[] = {
    "Hello World",
    "Concurrency in C",
    "Mutex and Barrier",
    "Data Processing Example"
};
int totalVowels = 0; / Variable para contar el total de vocales
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  

pthread_barrier_t barrier;

int count_vowels(const char* str) {
    int count = 0;
    char vowels[] = "aeiouAEIOU";
    for (int i = 0; str[i] != '\0'; i++) {
        for (int j = 0; vowels[j] != '\0'; j++) {
            if (str[i] == vowels[j]) {
                count++;
                break;
            }
        }
    }
    return count;
}

void* worker(void* arg) {
    int id = *(int*)arg;
    int localVowelCount = 0;

    printf("Worker %d: Processing string: \"%s\"\n", id, strings[id]);


    localVowelCount = count_vowels(strings[id]);

    pthread_barrier_wait(&barrier);


    pthread_mutex_lock(&mutex);
    totalVowels += localVowelCount;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    pthread_t workers[NUM_WORKERS];
    int worker_ids[NUM_WORKERS];

    pthread_barrier_init(&barrier, NULL, NUM_WORKERS);

    for (int i = 0; i < NUM_WORKERS; i++) {
        worker_ids[i] = i;
        if (pthread_create(&workers[i], NULL, worker, &worker_ids[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(workers[i], NULL);
    }


    pthread_barrier_destroy(&barrier);

    printf("\nTotal vowels in all strings: %d\n", totalVowels);

    return 0;
}
