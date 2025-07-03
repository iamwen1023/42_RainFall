#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int global_1 = 0;

void	greetuser(char *str) {
	char str_store[72];

	if (global_1 == 1) { // fi
		char *local_str = "Hyvää päivää ";
		memcpy(str_store, local_str, 19);

		// equivalent to:
		// *(int *)str_store = (int *) local_str;
		// *(int *)(str_store + 4) = (int *) (local_str + 4);
		// *(int *)(str_store + 8) = (int *) (local_str + 8);
		// *(int *)(str_store + 12) = (int *) (local_str + 12);
		// *(short *)(str_store + 16) = (short *) (local_str + 16);
		// *(str_store + 18) = *(local_str + 18);
	} else if (global_1 == 2) { // nl
		char *local_str = "Goedemiddag! ";
		memcpy(str_store, local_str, 14);

		// equivalent to:
		// *(int *)str_store = (int *) local_str;
		// *(int *)(str_store + 4) = (int *) (local_str + 4);
		// *(int *)(str_store + 8) = (int *) (local_str + 8);
		// *(short *)(str_store + 12) = (short *) (local_str + 12);
	} else if (global_1 == 0) { // nothing
		char *local_str = "Hello ";
		memcpy(str_store, local_str, 7);

		// equivalent to:
		// *(int *)str_store = (int *) local_str;
		// *(int *)(str_store + 4) = (int *) (local_str + 4);
	}

	strcat(str_store, str); // we can overflow the 79 characters.
	puts(str_store);
}

int main(int argc, char **argv) {
	if (argc != 3) return 1;

	char result[50];
	char str[76];

	bzero(str, 19);

	strncpy(str, argv[1], 40);
	strncpy(str + 40, argv[2], 32);

	char *lang_env = getenv("LANG");
	if (lang_env) {
		if (memcmp(lang_env, "fi", 2) == 0)
			global_1 = 1;
		else if (memcmp(lang_env, "nl", 2) == 0)
			global_1 = 2;
	}
	greetuser(memmove(result, str, 19));
}
