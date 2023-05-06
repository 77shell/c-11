#include <assert.h>

int main(int argc, char *argv[])
{
	int i {0};
	assert(!"hello");
	assert(i && "i == 0");
}
