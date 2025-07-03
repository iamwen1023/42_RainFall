#include <stdio.h>
#include <stdlib.h>

void v() {
    char buf[520]; // 0x208 = 520 bytes
    fgets(buf, 0x200, stdin); // Read up to 512 bytes into buf
    printf(buf);              // ⚠️ Dangerous: format string vulnerability!

    if (*(int *)0x804988c == 0x40) { 
        fwrite("Wait what?!\n", 1, 12, stdout);
        system("/bin/sh"); 
    }      
}

int main() {
    v();
    return 0;
}
