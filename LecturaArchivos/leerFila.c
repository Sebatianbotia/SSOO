#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int main(int argc, char **argv){

    FILE *f = fopen(argv[1], "r");
    if (f== NULL){
        perror("error al abrir el archivo");
        exit(1);
    }


    return 0;
}