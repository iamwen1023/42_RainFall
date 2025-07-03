#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void p()
{
    char buffer[104]; // 0x68 bytes
    fflush(stdout);
    gets(buffer);

    int value = *(int *)(buffer); // Assuming buffer contains an integer
    int check_value = value & 0xb0000000;

    if (check_value == 0xb0000000)
    {
        printf("(%p)\n"); // 0x8048620 
        exit(1);
    }
    else
    {
        puts(buffer);
        char *duplicate = strdup(buffer);
        free(duplicate); // Free the duplicated string
    }
}

#include <stdio.h>

void main()
{
    p();
    return 0;
}
