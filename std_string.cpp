#include <string>
#include <iostream>

int
main(int argc, char *argv[])
{
	char *p {nullptr};
	std::string s0 {p};
	std::string s {' '};
	//s.assign(30, ' ');
	s[0] = 'A';
	s[29] = 'X';
	s[30] = 'Z';
	std::cout << s << s.capacity() << std::endl;
	std::string s1 {"Access " + std::to_string(100) + "exceeds maximum length 63-byte"};
	std::cout << s1 << std::endl;
        return 0;
}
