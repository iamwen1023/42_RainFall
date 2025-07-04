#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void n() {
    system("/bin/cat /home/user/level7/.pass"); 
}

void m() {
    puts("Nope");  
}

int main(int argc, char **argv) {
    // char *buf = malloc(0x40);                // Allocate 64 bytes
    // void (**func_ptr)() = malloc(sizeof(void (*)(void))); // Allocate space for one function pointer
    char		*buf;
	func_ptr	*func;

	buf = malloc(64);
	func = malloc(4);

    *func = m;                           // Set the function pointer to point to m()

    strcpy(buf, argv[1]);                    // ⚠️ Unsafe: allows overflow if argv[1] > 63 chars

    (*func_ptr)();                           // Call the function via function pointer

    return 0;
}
