#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>


int main(){
    FILE *archivo = fopen("CharxChar.txt", "r");
    int c;
    while((c=fgetc(archivo))!=EOF){
        putchar(c);
    }
    printf("\n");

    return 0;
}
