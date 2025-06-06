#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char *input;
    int egid, euid;

    if (argc > 1)
    {
        input = argv[1];
        int value = atoi(input + 4);

        if (value == 423)
        { // 0x1a7 in decimal
            char *str = strdup("0x80c5348");
            if (str == NULL)
            {
                perror("strdup failed");
                return 1;
            }
            egid = getegid();
            euid = geteuid();
            setresgid(egid, egid, egid);
            setresuid(euid, euid, euid);
            execv(str, argv);
            free(str);
        }
        else
        {
            FILE *file = fopen("/path/to/file", "wb");
            if (file != NULL)
            {
                // (gdb) x/s 0x80c5350
                // 0x80c5350:	 "No !\n"
                fwrite("0x80c5350", 1, 5, file);
                fclose(file);
            }
        }
    }

    return 0;
}