#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 0;
    }

    int input = atoi(argv[1]);

    if (input == 0x1a7) {  // 0x1a7 == 423
        char *args[] = { strdup((char *)0x80c5348), NULL }; // /bin/sh

        gid_t gid = getegid();
        uid_t uid = geteuid();

        // setresgid(gid, gid, gid)
        setresgid(gid, gid, gid);

        // setresuid(uid, uid, uid)
        setresuid(uid, uid, uid);
        // actually the process is already running with euid=level1, due to the setuid bit

        execv((char *)0x80c5348, args); // /bin/sh
    } else {
        fwrite((void *)0x80c5350, 1, 5, *(FILE **)stderr);  // No !\n
    }

    return 0;
}