#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void o() {
    system("/bin/sh");
    _exit(1);
}

void n() {
    char buf[520]; // 0x208 = 520 bytes
    fgets(buf, 0x200, stdin);  // Read up to 512 bytes from stdin
    printf(buf);               // ⚠️ Format string vulnerability!
    exit(1);                   
}

int main() {
    n();
    return 0;
}
