#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include<fcntl.h>

void print_debug_tree(){
  char cmd[50];
   sprintf(cmd, "pstree -lp %d", getpid());
   system(cmd);
}

int hasKeyword( const char * str){

    const char * keywords[]={"int", "float", "return","if","else","for","while","printf"};
    int numkeywords= sizeof(keywords)/sizeof(keywords[0]);
    for(int i=0;i<8;i++){
        if(strstr(str,keywords[i])!=NULL){
            return 1;
        }
    }
    return 0;
}
int hasComment( const char * str){

    if(strstr(str,"//")!=NULL){
            return 1;
        }
    
    return 0;
}

int es_linea_vacia(char *str){

    for(int i = 0; str[i] != '\0'; i++){
        if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r'){
            return 0;
        }
    }

    return 1; 
}

int main( ){
    int nHijos = 3;
    int (*mat)[2] = (int(*)[])malloc((nHijos*2)*sizeof(int[2]));
    pid_t root = getpid();
    
    for (int j =0; j<nHijos*2;j++){
       if( pipe(mat[j])==-1){
            perror("error al crear la tuberia");
            exit(1);
       }
    }

    int i = 0;
    for (;i<nHijos;i++){
        if(!fork()){
            break;
        }
    }

    if(root == getpid()){
        print_debug_tree();

        for (int j=0;j<nHijos*2;j++){
            if(j%2==0){
                close(mat[j][0]);
            }
            else{
                close(mat[j][1]);
            }
        }

        FILE * code = fopen("code.txt", "r"); // abrir el archivo
        char buffer[100];
        while (fgets(buffer,sizeof(buffer),code)!=NULL)
        {
                    write(mat[0][1], buffer, strlen(buffer));
                    write(mat[1][1], buffer, strlen(buffer));
                    write(mat[2][1], buffer, strlen(buffer));
        }
        close(mat[0][1]);
        close(mat[1][1]);
        close(mat[2][1]);

        for(int j=0;j<nHijos;j++){
            wait(NULL);
        }

    }
    else{


        for(int j=0; j< nHijos*2;j++){
            if(j==i*2){
                close(mat[j][1]);
            }
            else if(j==i*2+1){
                close(mat[j][0]);
            }
            else{
                close(mat[j][0]);
                close(mat[j][1]);
            }
        }

        FILE * stream = fdopen(mat[i+i][0], "r");//acá volvemos el str un archivo, para poder leer la linea
        if(!stream){
            perror("error al abrir el archivo");
            return -1;
        }

        char line[100];

        if(i==0){// cuenta lineas vacias
            int contador1 = 0;
            size_t bytes;
            while(fgets(line, sizeof(line), stream) != NULL){
                if(es_linea_vacia(line)){
                    contador1 ++;
                }
            }
            

            printf("\n contador: %d\n", contador1);
            write(mat[i+i+1][1], &contador1, sizeof(int));
        }
        else if(i==1){// cuenta en cuantas lineas hay por lo menos una palabra clave
            int contador2 = 0;

            while(fgets(line, sizeof(line), stream) != NULL){
                
                //printf("Hijo 1 lee: %s", line);
                 if(hasKeyword(line)){
                    contador2 ++;;
                }
            }

            printf("\n contador 2: %d\n", contador2);
            write(mat[i+i+1][1], &contador2, sizeof(int));

        }


    }




    return 0;
}