#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int target = 0;  // this maps to 0x08049810

void p(char *input) {
    printf(input);  // VULNERABLE: no format string!
}

void n() {
    char buf[520];  // 0x208 bytes
    fgets(buf, 512, stdin);  // 0x200 = 512
    p(buf);

    if (target == 0x01025544) { //16930116
        system("/bin/cat /home/user/level5/.pass");
    }
}

int main() {
    n();
    return 0;
}
