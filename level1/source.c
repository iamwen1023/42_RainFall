#include <stdio.h>

#include <stdio.h>

void main() {
    char buffer[80]; // Allocating space for the buffer
    char *ptr = buffer + 16; // Pointer to the buffer

    gets(ptr); // Reading input into the buffer
}


#include <stdio.h>
#include <stdlib.h>

void run() {
    char *data = (char *)0x80497c0; //""
    char *filename = (char *)0x8048570;  //"Good... Wait what?\n"
    int size = 0x13;
    int count = 1;

    fwrite(data, size, count, filename);
    system("/bin/sh");
    return;
}