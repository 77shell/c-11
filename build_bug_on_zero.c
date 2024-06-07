#include <stdio.h>

#define BUILD_BUG_ON_ZERO(e)				\
	(						\
		(int)sizeof (struct { int:(-!!(e)); })	\
		)

int main()
{
	int i = 100;
	int j = !i;
	int k = !j;

	printf("i %d\tj == !i(%d)\tk == !j(%d)\n", i, j, k);
	
	 BUILD_BUG_ON_ZERO(0);
	_Static_assert(1);
	return 0;
}
