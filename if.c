#include <stdio.h>

int
main(int argc, char *argv[])
{
	int err = -2;
	if (err)
		puts("OK");
	else
		printf("%d\n", err);
}
