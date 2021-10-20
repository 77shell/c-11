
#include <string.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
	char *s = "012345";
	char buf[4];
	strncpy(buf, s, 4);
	for(int i=0; i<4; i++)
	{
		fprintf(stderr, "%c-%02x\n", buf[i], buf[i]);
	}
}
