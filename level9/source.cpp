#include <stdlib.h>
#include <string.h>

void *_ZN1NC1Ei(void *ptr, int nbr)
{
	*(int *)ptr = 0x8048848;
	*(int *)(ptr + 0x68) = nbr;
	return (ptr);
}

void *_ZN1N13setAnnotationEPc(char *dest, char *src)
{
	size_t src_length = strlen(src);
	return (memcpy(dest + 4, src, src_length));
}

int main(int argc, char **argv)
{
	if (argc <= 1)
		exit(1);
	char *ptr = new char[108];
	_ZN1NC1Ei(ptr, 5);
	void *ptr2 = new char[108];
	_ZN1NC1Ei(ptr2, 6);
	// The argv[1] length could exceed 108, and lead to an overflow
	_ZN1N13setAnnotationEPc(ptr, argv[1]);
	// Execute an allocated memory address
	return (**ptr2(ptr2, ptr));
}
