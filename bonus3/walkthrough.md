1. Vulnerability:

Here, we are taking the `argv[1]` of the user input, and pass it through `atoi`.

Then, the result of `atoi` is used to change a byte of the `str` string.
The index could be negative, or very large to for example modify any variable in the application.

It's a pretty dangerous behavior.

2. Bypass the condition:

To get a shell, we need to bypass that condition:
```c
if (!strcmp(str, argv[1]))
```

So, we need to have the password contained in `.pass` of `end` user,
to be equal to `argv[1]`.

That condition could be bypassed, only if the `str` is enought predictable.
And this is not the case, because flags are 64 bytes long.

But, if `str` has a length of 2 or less, that could be the case. We could "brute-force"
it. What if, the `str` is empty?

Yeah, that could be a good solution, have the `atoi(argv[1])` returning `0`,
which will write a null byte into the first character of `str`, thanks to this line:

```c
*(str + atoi(argv[1])) = 0;
```

Okay. But, if we send `0` as input, then `"0" != ""`. Yes. But if you send nothing to `atoi`,
then it's default behavior is to return you 0.

And then, `"" == ""` and `atoi("") == 0`.

3. Exploit:

```sh
bonus3@RainFall:~$ ./bonus3 ''
$ cd ../end
$ cat .pass
3321b6f81659f9a71c76616f606e4b50189cecfea611393d5d649f75e157353c
```