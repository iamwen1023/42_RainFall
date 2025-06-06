# 42_RainFall

```
qemu-system-x86_64 -cdrom ~/Downloads/RainFall.iso -m 4G -netdev user,id=net0,hostfwd=tcp::8022-:4242 -device virtio-net-pci,netdev=net0
```

```
ssh level0@127.0.0.1 -p 8022 
```

https://www.codeconvert.ai/free-converter


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