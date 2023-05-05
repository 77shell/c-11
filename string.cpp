#include <string>
#include <iostream>
using namespace std;


using msn_t = uint64_t;

union u64_t {
        unsigned long long val;
        uint8_t byte[8];
        uint16_t uint16[4];
        struct {
                uint32_t lo;
                uint32_t hi;
        } word;
};

msn_t get()
{
	u64_t msn;
	for (size_t i = 0; i < 4; i++)
		msn.uint16[i] = i * 0x1111;
	return msn.val;
}

int
main(int argc, char *argv[])
{
	cout << "unsigned long long " << sizeof(unsigned long long) << endl;

	char s[32];
	printf("MSN<%016llXh>\n", get());

        return 0;
}
