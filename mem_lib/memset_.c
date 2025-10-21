#include<string.h>
#include<stdio.h>

int main(){

    char str[] = "abcdef";
    char* tmp = "xyz";
    memset(str+2, 0x41, 2);
    printf("%s\n", str);
    memmove(str+2, "xyz", 3);
    printf("%s\n", str);


    char* line = "TRX:  cualquier vaina";
    if(memcmp(line, "TRX:", 4) == 0){

        
        printf("TRX: encontrado\n");
    };

    char* occ = NULL;
    if( (occ = memchr(line, 'v', strlen(line))) != NULL ){
        if(memcmp(occ, "vaina", 5) == 0){
            printf("palabra: vaina encontrada\n");
        }
    }

    return 0;
}