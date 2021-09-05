#include <iostream>

int main(int argc, char *argv[])
{
	unsigned short _2yyy {1920};
	_2yyy %= 100;
	std::cout << _2yyy << std::endl;
}
