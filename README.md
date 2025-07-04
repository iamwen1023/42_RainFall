# 42_RainFall

```
qemu-system-x86_64 -cdrom ~/Downloads/RainFall.iso -m 4G -netdev user,id=net0,hostfwd=tcp::8022-:4242 -device virtio-net-pci,netdev=net0
```

```
ssh level0@127.0.0.1 -p 8022 
```
https://www.codeconvert.ai/assembly-to-c-converter


📚 Function Stack Frame Structure (32-bit x86)
When a function is called (call main), the stack looks like this (top-down):

          ↑
          | High memory addresses
          |
    +-----------------+
    |   Arguments      |  ← Function arguments
    +-----------------+
    |  Return address  |  ← Pushed by `call`, used by `ret`
    +-----------------+
    | Previous EBP     |  ← Old base pointer (saved)
    +-----------------+ 
    |     EBP (now)    |  ← EBP now points here
    |  Local variables |  ← Space reserved by `sub esp, X`
    |       ...        |
    +-----------------+ 
          |  
          ↓
          | Low memory addresses
🔄 Function Call Flow (Simplified):
When calling a function:

push ebp        ; Save old base pointer
mov ebp, esp    ; Set EBP as base of new stack frame
sub esp, N      ; Make space for local variables
When returning from a function:

mov esp, ebp    ; Reset stack pointer
pop ebp         ; Restore old base pointer
ret             ; Return to caller (jumps to saved return address in EIP)
🟣 EIP — Instruction Pointer
Full Name: EIP (Extended Instruction Pointer), RIP in x86-64.
Purpose: Holds the address of the next instruction to execute.
Behavior: Automatically increased after each instruction, or modified by jmp, call, ret.

🟣 ESP — Stack Pointer
Full Name: ESP (Extended Stack Pointer), RSP in x86-64.
Purpose: Points to the top of the stack (grows downward).
Behavior:
push: decreases ESP (stack grows down).
pop: increases ESP (stack shrinks up).

🟣 EBP — Base Pointer
Full Name: EBP (Extended Base Pointer), RBP in x86-64.
Purpose: Serves as a fixed reference point in the current stack frame.
Behavior:
Used to access arguments (e.g., [ebp + 8]) and local variables (e.g., [ebp - 4]).
Easier to reference than ESP, which changes during function execution.

✅ Summary
EIP: Automatically controlled by CPU — executes instructions in order or jumps via call, jmp, ret.
ESP: Tracks the top of the stack, grows/shrinks with push, pop, or sub esp.
EBP: Stable reference within a function; used to access local variables and parameters.

🧠 Memory Layout Overview (Virtual Address Space)
+-----------------------+
|      Stack (grows down)   | ← ESP points here
|   Function params/local vars |
+-----------------------+
|      Heap (grows up)      |
|   malloc/new memory       |
+-----------------------+
|    Data Segment           |
|   Global/static variables |
+-----------------------+
|    Code Segment           |
|   Compiled instructions   |
+-----------------------+

2. Stack Protector (Stack Canary) Missing
Attack Cases:

Stack Buffer Overflow: Attackers exploit buffer overflow on the stack to overwrite the return address and execute arbitrary code.

Without canaries, attackers can more easily perform ROP (Return-Oriented Programming) attacks successfully.

Defense:

Stack Canary inserts a special value before the return address that gets checked before function returns. If altered (due to overflow), the program crashes, preventing exploitation.

3. NX (Non-Executable Memory)
Attack Cases:

Executing Shellcode on the Stack: If the stack is executable, attackers can inject and run malicious shellcode.

With NX disabled, attackers can write shellcode via overflow and then execute it.

Defense:

NX marks data memory areas (stack, heap, BSS) as non-executable, forcing attackers to use more complex techniques like ROP.

4. ASLR (Address Space Layout Randomization) Disabled
Attack Cases:

Memory Address Guessing: Fixed memory addresses make it easier for attackers to precisely locate key functions or data structures.

Combined with buffer overflow, attackers can jump directly to known addresses to execute malicious code.

Defense:

ASLR randomizes memory base addresses, increasing the difficulty of successful attacks.

5. PIE (Position Independent Executable) Missing
Attack Cases:

Fixed executable load addresses allow attackers to predict where the program loads in memory, making attacks easier, especially if ASLR is also disabled.

Defense:

PIE enables the executable to load at random addresses, working together with ASLR to thwart attacks.

6. RELRO Missing
Attack Cases:

GOT (Global Offset Table) Overwrite Attacks: Attackers modify GOT entries to redirect function calls to their malicious code.

Without RELRO, the GOT section is writable and vulnerable to hijacking.

Defense:

RELRO makes the GOT section read-only, preventing modification.

7. RPATH / RUNPATH Not Set
Attack Cases:

Misconfigured RPATH/RUNPATH can allow attackers to load malicious shared libraries instead of legitimate ones (DLL hijacking).

However, no RPATH usually means safer, avoiding unsafe dynamic library paths.

Defense:

Properly configure dynamic library paths to prevent loading malicious libraries.

8. Kernel Security Hardening (grsecurity / PaX / Kernel Heap Hardening) Missing
Attack Cases:

Kernel vulnerabilities can be exploited for privilege escalation (Kernel Exploits).

Without grsecurity and similar patches, kernel buffer overflows and other attacks are easier to succeed.

Defense:

grsecurity, PaX provide memory isolation, write protection, and other kernel-level hardening.

9. /dev/mem and /dev/kmem Access Not Restricted
Attack Cases:

Attackers use these device files to read/write kernel memory directly, bypassing security mechanisms or injecting malicious code.

Defense:

Restrict device access permissions to prevent unauthorized use.

check if a system (or a program’s data) is little-endian or big-endian

```sh
python -c "import sys; print(sys.byteorder)"
```

Little-endian means bytes are reversed:
0x08048444 → bytes \x44\x84\x04\x08
struct.pack("<I", addr) packs the address correctly for little-endian.
You can just remember that struct.pack("<I", 0x08048444) is equivalent to the byte string \x44\x84\x04\x08.

```py
def generate_pattern(length):
    pattern = []
    for i in range(length):
        # ex: Aa0, Aa1, Aa2 ... Ab0, Ab1 ...
        uppercase = chr(ord('A') + (i // 60) % 26)
        lowercase = chr(ord('a') + (i // 10) % 26)
        digit = str(i % 10)
        pattern.append(f"{uppercase}{lowercase}{digit}")
    return ''.join(pattern)[:length]

if __name__ == "__main__":
    length = 200  # length pattern
    print(generate_pattern(length))



def find_offset(subpattern):
    full_pattern = generate_pattern(200)
    return full_pattern.find(subpattern)

if __name__ == "__main__":
    length = 200
    pattern = generate_pattern(length)
    print(pattern)
    
    # for testing
    sub = "A7aA"
    offset = find_offset(sub)
    if offset != -1:
        print(f"Offset found at: {offset}")
    else:
        print("Subpattern not found!")
```