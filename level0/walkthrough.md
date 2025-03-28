
1. Run the Binary and Observe Behavior
Running the binary without arguments causes a segmentation fault:

```./level0```
```Segmentation fault (core dumped)```

We need to pass an argument to the binary to observe its behavior.


2. Testing with argument: To confirm the expected argument, try running the binary with various numbers. For example:

```./level0 123```

This will display the output no!

3. Disassembling the Binary: To understand the logic of how the program works, we need to disassemble it and inspect its code.

```gdb ./level0```

```(gdb) disassemble main```


4. Finding the Key Comparison: When inspecting the assembly code, look for the part where the program compares the passed argument to a constant value. Here's an example of a relevant code snippet:

```0x08048ed9 <+25>: cmp $0x1a7, %eax```
```0x08048ede <+30>: jne 0x8048f58```

```./level0 423```

```cat /home/user/level1/.pass```

```1fe8a524fa4bec01ca4ea2a869af2a02260d4a7d5fe7e7c24d8617e6dca12d3a```


