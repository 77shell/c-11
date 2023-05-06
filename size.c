#include <stdio.h>

int
main()
{
	printf("integer size %zd\n", sizeof(int));
	printf("long integer size %zd\n", sizeof(long));

	if (-1) {
		puts(__func__);
	}
}
