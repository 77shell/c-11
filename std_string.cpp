#include <string>
#include <iostream>
#include <cstring>

using namespace std;

std::string
get_substr()
{
	return string {"4928464614328390478"};
}

int
main(int argc, char *argv[])
{
	// char *p {nullptr};
	// std::string s0 {p};
	// std::string s {' '};
	// //s.assign(30, ' ');
	// s[0] = 'A';
	// s[29] = 'X';
	// s[30] = 'Z';
	// std::cout << s << s.capacity() << std::endl;
	// std::string s1 {"Access " + std::to_string(100) + "exceeds maximum length 63-byte"};
	// std::cout << s1 << std::endl;
	std::string mesg = "PSU <" + get_substr() + "> ";
	cout << mesg << endl;
	
	const char *sn_src = "Chungli R&D Team, Taiwan";
	std::string sn {sn_src, 24};
	cout << sn << endl;
        return 0;
}
