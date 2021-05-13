#include <stdio.h>

void swap(long long *a, long long *b){
    long long temp = *a;
    *a = *b;
    *b = temp;
}

int compare(long long a,long long b){
    if(a > b){
        return 1;
    }else{
        return 0;
    }
}

void percolate(long long *a, long long size) {
    long long i;
    for (i = 0; i < size - 1; i++) {
        if (compare(a[i],a[i+1])) {
            swap(a + i, a + i + 1);
        }
    }
}

void bubblesort(long long *a, long long size){
    for(long long i = 0; i < size -1; i++){
        percolate(a,size);
    }
}

void fill(long long *a, long long size){
    long long i;
    for(i = 0; i < size; i++){
        a[i] = size - i;
    }
}

int main() {
    long long a[10];
    a[0] = 10;
    a[1] = 9;
    a[2] = 8;
    a[3] = 7;
    a[4] = 5;
    a[5] = 6;
    a[6] = 4;
    a[7] = 3;
    a[8] = 2;
    a[9] = 1;
    fill(a,10);
    bubblesort(a,10);
    printf("Hello, World!\n");
    return 0;
}