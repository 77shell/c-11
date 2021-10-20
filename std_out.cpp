#include <string>
#include <iostream>

int
main(int argc, char *argv[])
{
	uint8_t c {10};
	std::cout << std::hex << std::showbase << static_cast<int>(c) << std::endl;
        return 0;
}
