#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Global variables (based on memory addresses in assembly)
char *ptr1;  // 0x8049aac
char *ptr2;  // 0x8049ab0
FILE *stream; // 0x8049aa0 (likely stderr)

int main() {
    char buffer[128];  // Stack buffer (0xa0 = 160 bytes allocated, ~128 for buffer)
    
    while (1) {
        // Print prompt with two global pointers
        printf("%s %s ", ptr2, ptr1);
        
        // Read input line
        if (fgets(buffer, 128, stdin) == NULL) {
            break;  // Exit on EOF
        }
        
        if (strncmp(buffer, "auth ", 5) == 0) {
            // Allocate 4 bytes and store pointer
            ptr1 = malloc(4);
            *((int*)ptr1) = 0;  // Initialize to 0
            
            // Get the argument after "auth  "
            char *arg = buffer + 5;
            
            // Check length constraint (must be <= 30 characters)
            if (strlen(arg) <= 30) {
                strcpy(ptr1, arg);  // Unsafe strcpy - potential buffer overflow!
            }
        }
        
        else if (strncmp(buffer, "reset", 4) == 0) {
            if (ptr1) {
                free(ptr1);
            }
        }
        
        else if (strncmp(buffer, "service", 7) == 0) {
            char *arg = buffer + 7;
            ptr2 = strdup(arg);  
        }
        
        // Check for "login" command
        else if (strncmp(buffer, "login", 5) == 0) {
            // Check if there's data at offset 0x20 (32) from ptr1
            if (ptr1 && *((int*)(ptr1 + 32)) != 0) {
                system("/bin/sh");  // Launch shell - privilege escalation!
            } else {
                fwrite("Access denied\n", 1, 10, stderr);
            }
        }
    }
    
    return 0;
}