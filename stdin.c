#include <stdio.h>

int
main()
{
	for (;;)
	{
		int c = getchar();
		if (c == -1)
			continue;
		printf("got char: %c %d\n", c, c);
		// putchar(c);
	}
}
