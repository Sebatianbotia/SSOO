#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
int main(){

    FILE* f = fopen("in.txt", "r");
    if(!f){
        perror("file");
        return 1;
    }

    char buff[1024];
    while( fgets(buff, sizeof(buff), f) != NULL ){

        if(memcmp(buff, "TRX:", 4) == 0){
            char* tmp = memchr(buff, '-', strlen(buff));
            if(tmp != NULL && (memcmp(tmp+1, ">", 1) == 0)){
                printf("TRX: encontrado, primer filtro completado\n");
            }else{
                printf("Linea no valida\n");
            } 
        }else{
            printf("Linea no valida\n");
        }

        char* v = memchr(buff+5, ':' ,strlen(buff)-5);
        if(v){
            char* n = (char*)malloc(1);
            int len_n = 0;
            int is_num = 1;
            for(char* cc = v; *cc != '\0'; cc++){
                if(is_num){
                    if(isdigit(*cc)){
                        n[len_n] = *cc;
                        n = (char*)realloc(n, len_n+2);
                        len_n++;
                        n[len_n] = '\0';
                    }
                }
            }

            int res = atoi(n);
            printf("%d\n", res);

            if(strstr(buff, "hacker") || strstr(buff, "cashout") || strstr(buff, "vault")){
                printf("linea invalida, palabra sospechosa\n");
            }
        }
    }

    return 0;
}