#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

int main() {
    printf("[max]: %p\n", max); 
    printf("Size of max: %ld, Void: %ld \n", sizeof(max), sizeof(void));
    printf("[max_pointer]: %p\n", &max); 
    printf("Size of max pointer: %ld, Void *: %ld \n", sizeof(&max), sizeof(void *));

    int(*p)(int, int); // 函数指针
    int(*pp)(int, int);
    p = max; // 不同的赋值方式
    pp = &max;
    printf("[p]: %p, size: %ld\n", p, sizeof(p)); 
    printf("[*pp]: %p, size: %ld\n", pp, sizeof(pp)); 
    int a = 1;
    int b = 2;
    printf("*p func: %d\n", p(a, b));
    printf("*pp func: %d\n", pp(a, b));
}
