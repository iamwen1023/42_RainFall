#include <stdio.h>
#include <stdlib.h>

void v() {
    char buf[520]; // 0x208 = 520 bytes
    fgets(buf, 0x200, stdin); // Read up to 512 bytes into buf
    printf(buf);              // ⚠️ Dangerous: format string vulnerability!

    if (*(int *)0x804988c == 0x40) {
        int fd = *(int *)0x8049880;
        fwrite((void *)0x8048600, 1, 12, fd); // write 12 bytes from 0x8048600 to fd
        system((char *)0x804860d);           // execute command at 0x804860d
    }
}

int main() {
    v();
    return 0;
}
