#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<signal.h>


#define NO_BURN 0
#define BURNING 1
#define BURNED -1
#define PROP 1
#define CONS 2
#define REV 3

void __kill(pid_t pid, int sig){
    usleep(1000000);
    kill(pid, sig);
}

typedef struct Control
{
    int phase;
    int end;
    int flag;
} Control;

void _log(int idx, const char* msg){
    printf("[%d] -> %s\n", idx, msg);
}

void handler(int s){}

int** read_file(const char* fname, int* f, int* c, int* h, int* n_h){
    FILE* fp = fopen(fname, "r");
    if(!fp){
        perror("file");
        abort();
    }
    //int hours, childs, rows, col;
    fscanf(fp, "%d", h);
    fscanf(fp, "%d", n_h);
    fscanf(fp, "%d", f);
    fscanf(fp, "%d", c);

    signal(SIGUSR1, handler);

    int** m = (int**)malloc(sizeof(int*)*(*f));
    if(!m){
        perror("memory");
        abort();
    }

    for(int i = 0; i < *f; i++){
        m[i] = (int*)malloc(sizeof(int)*(*c));
        if(!m[i]){
            perror("memory");
            abort();
        }
    }

    for(int i = 0; i < *f; i++){
        for(int j = 0; j < *c; j++){
            fscanf(fp, "%d", &m[i][j]);
        }
    }
    
    return m;
}

void move_matrix(int** SRC, int** DEST, int f, int c){
    for(int i = 0; i < f; i++){
        for(int j = 0; j < c; j++){
            DEST[i][j] = SRC[i][j];
        }
    }
}

void log_matrix(int** M, int f, int c){
    for(int i = 0; i < f; i++){
        for(int j = 0; j < c; j++){
            printf("%d ", M[i][j]);
        }
        printf("\n");
    }
}

int search_neighbors(int** A, int f, int c, int x, int y, int target){

    int i_x = x - 1;
    int e_x = x + 1;
    int i_y = y - 1;
    int e_y = y + 1;

    int found = 0;

    //found = (y-1 >= 0 && A[x][y-1] == target) ? found+1 : 0; Asi para arriba, abajo, izq y der

    for(int i = i_y; i <= e_y; i++){
        for(int j = i_x; j <= e_x; j++){
            if(j >= 0 && i >=0 && i < f && j < c && i != x && j != y){
                printf("[%d][%d] -> %d\n", i, j, A[i][j]);
                if(A[i][j] == target) found++;
            }
        }
    }

    return found;
}

void matrix_propagation(int** A, int** R, int f, int c, int start, int end){

    int burned = 0;

    for(int i = start; i < end; i++){
        for(int j = 0; j < c; j++){
            burned = search_neighbors(A, f, c, i, j, BURNING);
            //printf("Vecinos quemados -> %d\n", burned);
            if(burned >= 2){
                R[i][j] = BURNING;
                //printf("R[%d][%d] -> %d\n", i, j, R[i][j]);
            }
        }
    }

}

void matrix_consume(int** A, int** R, int f, int c, int start, int end){

    for(int i = start; i < end; i++){
        for(int j = 0; j < c; j++){
            if(A[i][j] == 1) R[i][j] = -1;
        }
    }
}

int main(int argc, char** argv){

    if(argc < 2){
        perror("args");
        printf("Faltan argumentos\n");
        return 1;
    }
    int rows, cols, childs, hours;

    int** mat = read_file(argv[1], &rows, &cols, &hours, &childs);

    int shm_control = shmget(IPC_PRIVATE, sizeof(Control), 0666 | IPC_CREAT);
    Control* ctrl = shmat(shm_control, NULL, 0);

    ctrl->end = 0;
    ctrl->flag = 0;
    ctrl->phase = PROP;

    int shmid = shmget(IPC_PRIVATE, sizeof(int)*rows*cols + rows*sizeof(int*), 0666 | IPC_CREAT);
    void* base = shmat(shmid, NULL, 0);

    int** M = (int**)base;
    int* d = (int*)((char*)base + sizeof(int*)*rows);
    for(int i = 0; i < rows; i++){
        M[i] = &d[i*cols];
    }

    int shmidR = shmget(IPC_PRIVATE, sizeof(int)*rows*cols + rows*sizeof(int*), 0666 | IPC_CREAT);
    void* Rbase = shmat(shmidR, NULL, 0);

    int** R = (int**)Rbase;
    int* dR = (int*)((char*)Rbase + sizeof(int*)*rows);
    for(int i = 0; i < rows; i++){
        R[i] = &dR[i*cols];
    }

    move_matrix(mat, M, rows, cols);
    move_matrix(mat, R, rows, cols);

    pid_t* vec_childs = (pid_t*)malloc(sizeof(pid_t)*childs);
    if(!vec_childs){
        perror("memory");
        return 1;
    }

    pid_t root = getpid();

    int idx = 0;
    for(; idx < childs; idx++){
        if(!(vec_childs[idx] = fork()))
            break;
    }

    int current_hour = 0;
    if(root == getpid()){
        while(current_hour < hours){
            //printf("hora: %d\n", current_hour);
            //Fase 1
            //printf("Fase 1\n");
            ctrl->phase = PROP;
            for(int i = 0; i < childs; i++){
                //printf("Despertando a %d\n", i);
                __kill(vec_childs[i], SIGUSR1);
            }
            //printf("Padre durmiendo\n");
            pause();
            
            ctrl->flag=0;
            //Fase 2
            //printf("Fase 2\n");
            ctrl->phase = CONS;
            for(int i = 0; i < childs; i++){
                //printf("Despertando a %d\n", i);
                usleep(10000);
                __kill(vec_childs[i], SIGUSR1);
            }
            //printf("Padre durmiendo\n");

            pause();
            //_log(-1, "checkpoint");
            ctrl->flag=0;

            //Fase 3
            //printf("Fase 3\n");
            log_matrix(R, rows, cols);
            move_matrix(R, M, rows, cols);
            current_hour++;
            ctrl->flag=0;
        }
        ctrl->end = 1;
        for(int i = 0; i < childs; i++){
            printf("Despertando a %d\n", i);
            __kill(vec_childs[i], SIGUSR1);
        }
        for(int i = 0; i < childs; i++) wait(NULL);

    }else{
        int delta = childs / rows;
        int start = delta * idx;
        int end = (idx == childs-1) ? rows : start + delta;

        while(1){
            //if(idx == 0) printf("Hora %d - Hijo 0 durmiendo\n", current_hour);
            pause();
            if(ctrl->end == 1) break;
            //if(idx == 0) printf("Hora %d - Hijo 0 despierto\n", current_hour);
            if(ctrl->phase == PROP){
                //if(idx == 0) printf("hijo 0: propagacion\n");
                matrix_propagation(M, R, rows, cols, start, end);
            }else if (ctrl->phase == CONS){
                //if(idx == 0) printf("hijo 0: consumir\n");
                matrix_consume(M, R, rows, cols, start, end);
            }
            //if(idx == 1) printf("hijo 1: flag %d\n", ctrl->flag);
            ctrl->flag++;
            printf("%d flag -> %d\n", idx, ctrl->flag);
            if(ctrl->flag == childs){
                //printf("%d Despertado al padre\n", idx);
                usleep(1000);
                kill(getppid(), SIGUSR1);
            }
            //if(ctrl->end) break;
        }
    }
    return 0;
}