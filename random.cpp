#include <cstdlib>
#include <iostream>
#include <ctime>

using namespace std;

int
main(int argc, char *argv[])
{
	srand(std::time(nullptr));
	cout << rand() << endl;
	cout << static_cast<uint16_t>(std::time(nullptr)) << endl;
}
