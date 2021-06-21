#include <cstdint>
#include <stdio.h>
#include <bitset>
#include <iostream>
#include <climits>

int main(int argc, char *argv[])
{
	std::bitset<128> buf;
	std::bitset<128> bl {ULLONG_MAX};
	std::cout << bl << std::endl;
	std::cout << ULLONG_MAX << std::endl;
	std::cout << std::hex << ULLONG_MAX << std::endl;
	std::cout << sizeof(ULLONG_MAX) << std::endl;
	buf = bl;
	std::cout << buf << std::endl;
}
