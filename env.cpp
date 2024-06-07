#include <stdlib.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	if (argc == 2) {
		char *var {getenv(argv[1])};
		cout << argv[1] << "\t" << var << endl;
	}
}

