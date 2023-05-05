#include <cstring>
#include <stdio.h>

int
src_is_shorter_than_dest()
{
        constexpr size_t dest_len {32};
        const char *src {"12345678"};
        char dest[dest_len]; /* null character */

	for (int i {}; i < dest_len; i++)
		dest[i] = 'X';

	size_t len {12};
	strncpy(dest, src, len);
	printf("Copy string len %zd\n", len);
	for (int i {}; i < dest_len; i++)
		printf("%02Xh-%c ", dest[i], dest[i]);

        return 0;
}

int
main(int argc, char *argv[])
{
        constexpr size_t dest_len {32};
        const char *src {"123456789ABCEDF"};
        char dest[dest_len]; /* null character */

	for (int i {}; i < dest_len; i++)
		dest[i] = 'X';

	size_t len {10};
	strncpy(dest, src, len);
	printf("Copy string len %zd\n", len);
	for (int i {}; i < dest_len; i++)
		printf("%02Xh-%c ", dest[i], dest[i]);

	puts("\nTest case 2: src is shorter than dest");
	src_is_shorter_than_dest();
        return 0;
}
