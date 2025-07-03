#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char flag[68];

void m() {
    time(NULL);
    printf("your token is at %p\n", flag);
}

int main(int argc, char **argv) {
    int *a = malloc(8);
    a[0] = 1;
    a[1] = malloc(8);

    int *b = malloc(8);
    b[0] = 2;
    b[1] = malloc(8);
    strcpy((char *)a[1], argv[1]);
    strcpy((char *)b[1], argv[2]);

    FILE *f = fopen("/home/user/level8/.pass", "r"); 
    // 0x80486e9: "/home/user/level8/.pass"
    // 0x80486eb: "r"
    // with x/s 0x80486e9
    fgets(flag, 68, f);

    puts("~~");
    return 0;
}
