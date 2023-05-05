#include <iostream>
#include <cstring>
#include <unistd.h>

using namespace std;

void
f()
{
	constexpr size_t len {16};
	char ary[len];
	char s[] = "helloj;lakjdf;lajfd;ladjf;laajdfqpiourep";

	cout << sizeof s << endl;
	
	memset(ary, 0, len);
	ary[16] = 0;
	strcpy(ary, s);
}

char
f_access_invalid()
{
	char *p = (char*)malloc(10 * sizeof(char));
	free(p);
	return p[1];
}

int
main(int argc, char *argv[])
{
	f();
	char c = f_access_invalid();
	return 0;
}
