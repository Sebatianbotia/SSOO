#include <stdio.h>
#include <stdlib.h>

int cmp_int(const void *a, const void *b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void){
    int v[] = {5, -3, 9, 9, 2, 0};
    size_t n = sizeof v / sizeof v[0]; //tamaño en bytes del vector / tamaño en bytes del tipo del primer elemento( int = 4 bytes)
    qsort(v, n, sizeof v[0], cmp_int);
    for (size_t i=0;i<n;++i) printf("%d ", v[i]);
    puts("");
}