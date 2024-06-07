#include <iostream>
#include <array>

using namespace std;

int main(int argc, char *argv[])
{
	std::array<char, 24> sn {};
	cout << "array size: " << sn.size() << endl;
	cout << "array max size: " << sn.max_size() << endl;
}
