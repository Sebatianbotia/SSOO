#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_WORKERS 4
#define MAX_STRING_LENGTH 100
#define MAX_LINES 10 


char* strings[MAX_LINES]; 
int totalVowels = 0;
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

    printf("Trabajador %d: procesa cadena: \"%s\"\n", id, strings[id]);

    
    localVowelCount = count_vowels(strings[id]);

    pthread_barrier_wait(&barrier);

    pthread_mutex_lock(&mutex);
    totalVowels += localVowelCount;
    pthread_mutex_unlock(&mutex);

    return NULL;
}


void read_file_and_load_strings(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error al abrir el archivo");
        exit(1);
    }

    char line[MAX_STRING_LENGTH];
    int index = 0;

    while (fgets(line, sizeof(line), file) && index < MAX_LINES) {

        line[strcspn(line, "\n")] = '\0';//devuelve el índice donde se encuentra el salto de línea

        strings[index] = strdup(line);  // Copia la cadena a la matriz
        //strdup Devuelve un puntero a una nueva cadena en memoria dinámica (heap) que contiene una copia de la cadena original
        index++;
    }

    fclose(file);
}

int main() {
    pthread_t workers[NUM_WORKERS];
    int worker_ids[NUM_WORKERS];

    const char* filename = "strings.txt"; 
    read_file_and_load_strings(filename);

    pthread_barrier_init(&barrier, NULL, NUM_WORKERS);

    for (int i = 0; i < NUM_WORKERS; i++) {
        worker_ids[i] = i;
        if (pthread_create(&workers[i], NULL, worker, &worker_ids[i]) != 0) {
            perror("Error");
            return 1;
        }
    }

    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(workers[i], NULL);
    }

    pthread_barrier_destroy(&barrier);

    printf("Total e vocales: %d\n", totalVowels);

    for (int i = 0; i < MAX_LINES; i++) {
        free(strings[i]);
    }

    return 0;
}
