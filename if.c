#include <stdio.h>

int
main(int argc, char *argv[])
{
	int err = -2;
	if (err)
		puts("OK");
	else
		printf("%d\n", err);

	int i = 10;
	int j = i % 10;
	printf("i %d j %d\n", i, j);
}
