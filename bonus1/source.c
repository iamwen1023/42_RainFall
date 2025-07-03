#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int	main(int argc, char **argv)
{
	char str[40];
	int arg1 = atoi(argv[1]);
	
	if (arg1 > 9) return 1;

	memcpy(str, argv[2], (arg1 * 4));

	if (arg1 != 0x574f4c46) return 0;

	execl("/bin/sh", "sh", 0);
}