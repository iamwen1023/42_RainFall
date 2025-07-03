#include <stdio.h>
#include <string.h>
#include <unistd.h>

char *p(char *str, char *to_print) {
	char buff[0x1008];

	puts(to_print);
	read(0, buff, 0x1000);
	*strchr(buff, '\n') = 0;
	return (strncpy(str, buff, 20));
}

char *pp(char *str) {
	char str2[20];
	char str3[28];

	p(str2, " - ");
	p(str3, " - ");
	strcpy(str, str2);

	char *end = (str + strlen(str));
	*end = ' ';
	*(end + 1) = 0;

	return (strcat(str, str3));
}

int main(void) {
	char str[42];

	pp(str);
	puts(str);
	return (0);
}