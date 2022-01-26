#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
	printf("Time zone: %d %ld %s %s\n", daylight, timezone, tzname[0], tzname[1]);
	return 0;
}
