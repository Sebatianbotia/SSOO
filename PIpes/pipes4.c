#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>

//sizeof(MyStruct)

#define NH 3

char* read_file(const char* fname){

    FILE* f = fopen(fname, "r");
    if(!f){
        perror("file");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char* buffer = (char*)malloc(size);
    if(!buffer){
        perror("memory");
        fclose(f);
        return NULL;
    }

    size_t bytes_readed = fread(buffer, 1, size, f);
    if(bytes_readed != size){
        perror("file");
        free(buffer);
        fclose(f);
        return NULL;
    }

    fclose(f);
    return buffer;

}

int main(){

    pid_t padre = getpid();

    int fd[NH*2][2];

    for(int i = 0; i < NH*2; i++){
        if((pipe(fd[i])) == -1){
            perror("pipe");
            return 1;
        }
    }

    int i = 0; 
    for(; i < NH; i++){
        if(!fork()) break;
    }

    /*
        P
     // || \\
    h1  h2  h3
    */

    if(padre == getpid()){

        close(fd[0][0]);
        close(fd[1][1]);
        close(fd[2][0]);
        close(fd[3][1]);
        close(fd[4][0]);
        close(fd[5][1]);

        char* src = read_file("in.txt");
        int size = strlen(src);

        write(fd[0][1], src, strlen(src));
        write(fd[2][1], src, strlen(src));
        write(fd[4][1], src, strlen(src));

        int code_size = 0;
        read(fd[1][0], &code_size, sizeof(int)); //Hijo 1: Lineas de codigo encontradas
        int key_size = 0;
        read(fd[3][0], &key_size, sizeof(int)); //Hijo 2: Cantidad de palabras clave encontradas
        int comment_size = 0;
        read(fd[5][0], &comment_size, sizeof(int)); //Hijo 3: Cantidad de comentarios

        for(int i = 0; i < NH; i++){
            wait(NULL);
        }
        printf("Lineas leidas: %d\nCantidad de palabras clave: %d\nCantidad de comentarios: %d\n", code_size, key_size,comment_size);
    }else{
        char buff[1000];
        if(i == 0){
            close(fd[2][0]);
            close(fd[2][1]);
            close(fd[3][0]);
            close(fd[3][1]);
            close(fd[4][0]);
            close(fd[4][1]);
            close(fd[5][0]);
            close(fd[5][1]);

            close(fd[0][1]);
            close(fd[1][0]);

            int br = read(fd[0][0], &buff, sizeof(buff));
            buff[strlen(buff)+1] = '\0';
            
            int line = 1;
            for(char* s = buff; *s != '\0'; s++){
                if (*s == '\n') line++;
            }
            
            write(fd[1][1], &line, sizeof(int));
            
        //    int c = 1;
        //    int size = strlen(buff);
        //    while(c <= size){
        //        if(buff[c-1] == '\n' && buff[c] != '\n') line++;
        //        c++;
        //   }

        }else if(i == 1){

            close(fd[0][0]);
            close(fd[0][1]);
            close(fd[1][0]);
            close(fd[1][1]);
            close(fd[4][0]);
            close(fd[4][1]);
            close(fd[5][0]);
            close(fd[5][1]);

            close(fd[2][1]);
            close(fd[3][0]);
                        
            int br = read(fd[2][0], &buff, sizeof(buff));
            buff[strlen(buff)+1] = '\0';

            const char* keywords[] = {"int", "float", "return", "if", "else", "for", "while", "printf"};

            const char* del = " ";
            char* tk;
            int n_k = sizeof(keywords) / sizeof(keywords[0]);
            // int* 
            int kw = 0;
            tk = strtok(buff, del);
            do{
                for(int j = 0; j < n_k; j++){
                    if(memcmp(tk, keywords[j], strlen(keywords[j])) == 0) kw++;
                }
            }
            while((tk = strtok(NULL, del)) != NULL);
            write(fd[3][1], &kw, sizeof(int));

        }else if(i == 2){

            close(fd[0][0]);
            close(fd[0][1]);
            close(fd[1][0]);
            close(fd[1][1]);
            close(fd[2][0]);
            close(fd[2][1]);
            close(fd[3][0]);
            close(fd[3][1]);

            close(fd[4][1]);
            close(fd[5][0]);

            read(fd[4][0], &buff, sizeof(buff));
            buff[strlen(buff)+1] = '\0';

            int n_c = 0;
            int c = 0;
            int size = strlen(buff);
            
            for(int j = 1; buff[j] != '\0'; j++){
                if(buff[j-1] == '/' && buff[j] == '/'){
                    n_c++;
                    while(buff[j] != '\n' && buff[j] != '\0') j++;
                }
            }
            write(fd[5][1], &n_c, sizeof(int));
        }
    }
    return 0;
}