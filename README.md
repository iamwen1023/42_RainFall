# 42_RainFall

```
qemu-system-x86_64 -cdrom ~/Downloads/RainFall.iso -m 4G -netdev user,id=net0,hostfwd=tcp::8022-:4242 -device virtio-net-pci,netdev=net0
```

```
ssh level0@127.0.0.1 -p 8022 
```