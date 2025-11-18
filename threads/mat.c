#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

#define NORM 0
#define SCALE 1


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int stage = 0;

int rows = 0;
int cols = 0;

int c = 0;

int nthreads;

pthread_barrier_t barrier;

float** m = NULL;
float greater = __FLT_MIN__;
typedef struct arg {
    int ini;
    int fin;
} arg;

void panic(const char* reason){
    perror(reason);
    abort();
}


void* processing(void* args){
    arg* a = (arg*)args;

    do{
        if(stage == NORM){
            float sum;
            for(int i = a->ini; i < a->fin; i++){
                sum = 0;
                for(int j = 0; j < cols; j++){
                    sum+=m[i][j];
                }
    
                for(int j = 0; j < cols; j++){
                    m[i][j] /= sum;
                }
            }
    
        }else if (stage == SCALE){

            for(int i = a->ini; i < a->fin; i++){
                for(int j = 0; j < cols; j++){
                    if(greater < m[i][j]){
                        pthread_mutex_lock(&mutex);
                        greater = m[i][j];
                        pthread_mutex_unlock(&mutex);
                    }
                }
            }

            pthread_barrier_wait(&barrier);

            for(int i = a->ini; i < a->fin; i++){
                for(int j = 0; j < cols; j++){
                    if(greater < m[i][j]){
                        m[i][j] /= greater;
                    }
                }
            }

        }else{
            break;
        }
        c++;
        if(c == nthreads){
            stage++;
            c = 0;
        }
        pthread_barrier_wait(&barrier);
    } while(1);

    pthread_exit(NULL);
}

void read_file(const char* fname){
    FILE* f = fopen(fname, "r");
    if(!f){
        panic("memory");
    }

    fscanf(f, "%d %d", &rows, &cols);

    m = (float**)malloc(sizeof(float*)*rows);

    for(int i = 0; i < rows; i++){
        m[i] = (float*)malloc(sizeof(float)*cols);
    }

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            fscanf(f, "%f", &m[i][j]);
        }
    }
}

int main(int argc, char**argv){

    if(argc < 3){
        panic("args");
    }

    nthreads = atoi(argv[2]);
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t)*nthreads);

    pthread_barrier_init(&barrier, NULL, nthreads);

    read_file(argv[1]);
    
    int delta = rows / nthreads;
    for(int i = 0; i < nthreads; i++){
        arg* a = (arg*)malloc(sizeof(arg));
        a->ini = i*delta;
        a->fin = (i == nthreads - 1) ? rows : a->ini + delta;
        pthread_create(&threads[i], NULL, processing, (void*)a);
    }

    for(int i = 0; i < nthreads; i++){
        pthread_join(threads[i], NULL);
    }

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            printf("%.2f ", m[i][j]);
        }
        printf("\n");
    }

    return 0;
}