#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(){

    int fd[2];

    pipe(fd);

    int num = 10;
    int res;
    write(fd[1], &num, sizeof(int));
    read(fd[0], &res, sizeof(int));

    printf("res: %d\n", res);
    return 0;
}