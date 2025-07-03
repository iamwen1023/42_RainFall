# 42_RainFall

```
qemu-system-x86_64 -cdrom ~/Downloads/RainFall.iso -m 4G -netdev user,id=net0,hostfwd=tcp::8022-:4242 -device virtio-net-pci,netdev=net0
```

```
ssh level0@127.0.0.1 -p 8022 
```
https://www.codeconvert.ai/assembly-to-c-converter


函式開始時（call main）
          ↑
          | 高位址（記憶體）
          |
    +-----------------+
    |  傳進來的參數   |
    +-----------------+
    |      返回位址   |
    +-----------------+
    |      舊的 EBP   | <- EBP 舊值（進入函式前儲存）
    +-----------------+ <- EBP（現在值）
    |  區域變數區塊   |
    |     ...         |
    +-----------------+ <- ESP（現在指向 stack 頂端）
          |
          ↓
          | 低位址（記憶體）


執行流程簡單來說：
呼叫函式時會 push ebp → 把前一層的 ebp 儲存下來

然後 mov ebp, esp → 把 ebp 設為現在的堆疊頂端（也就是這個函式的起點）

用 esp 再往下開空間存區域變數

🟣 EIP — Instruction Pointer
全名：EIP (Extend Instruction Pointer)，在 32-bit x86 中是 EIP，在 64-bit x86-64 中是 RIP。
用處：記住 CPU 下一條要執行的指令。
每當 CPU 執行一條指令後，EIP 就會 自动增加，或者遇到 jmp、call 等跳轉時，EIP 就會修改到指定的位址。

🟣 ESP — Stack Pointer
全名：ESP (Extend Stack Pointer)，在 32-bit x86 中是 ESP，64-bit 中是 RSP。
用處：永遠指向當前 stack 的 "top"（棧頂）。
每當我們 push 一個變量到 stack，ESP 就會 下降（由高位記憶體移到低位），而當我們 pop 一個變量出去，ESP 就會 上升。

🟣 EBP — Base Pointer
全名：EBP (Extend Base Pointer)，在 32-bit x86 中是 EBP，64-bit 中是 RBP。
用處：通常用 EBP 作為當前 stack frame 的“基準點”，方便我們透過 EBP + offset 訪問當地變量與參量。
一般 function call 的序列是：

```
push ebp          ; 儲存舊的 base pointer
mov ebp, esp      ; 設定 ebp 為當前 stack 的起點
sub esp, xxx      ; 為當地變量空出空間
```
函式結束時：
```
mov esp, ebp     ; Stack pointer 回到 base
pop ebp          ; 還原舊 base pointer
ret               ; 返回到 EIP 儲存的 return address
```

🟣 小結：
✅ EIP: CPU 自動依 ret 或 call 控制，我們不需要自己修改。
✅ ESP: 每次 push 或 pop 都由 CPU 或 compiler 控制，依 stack 的變化 由高變低 or 由低變高。
✅ EBP: 每遇到一個新的 stack frame，compiler 會 push ebp; mov ebp, esp;，然後以它為“錨點”取變量與參量。


簡單示意圖（虛擬內存結構）
+-----------------------+
|      Stack (向下增長)     | ← ESP 指向這裡
|                       |
|   函數參數、局部變數      |
+-----------------------+
|       Heap (向上增長)      |
|                       |
|  動態分配的記憶體區塊      |
+-----------------------+
|     Data Segment       |
| (全域變數、靜態變數、常量) |
+-----------------------+
|     Code Segment       |
|     (程式指令碼)        |
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
        # 這裡用三個字元組成一組：一個大寫字母 + 一個小寫字母 + 一個數字
        # 例如 Aa0, Aa1, Aa2 ... Ab0, Ab1 ...
        uppercase = chr(ord('A') + (i // 60) % 26)
        lowercase = chr(ord('a') + (i // 10) % 26)
        digit = str(i % 10)
        pattern.append(f"{uppercase}{lowercase}{digit}")
    return ''.join(pattern)[:length]

if __name__ == "__main__":
    length = 200  # 你想要多長的 pattern 就改這裡
    print(generate_pattern(length))



def find_offset(subpattern):
    full_pattern = generate_pattern(200)
    return full_pattern.find(subpattern)

if __name__ == "__main__":
    length = 200
    pattern = generate_pattern(length)
    print(pattern)
    
    # 假設 EIP 解析為 "A7aA" 就輸入這裡測試
    sub = "A7aA"
    offset = find_offset(sub)
    if offset != -1:
        print(f"Offset found at: {offset}")
    else:
        print("Subpattern not found!")
```