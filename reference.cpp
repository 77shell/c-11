#include <iostream>
#include <map>

using namespace std;

struct C {
	C() : i0 {10}
		{}
	
	~C() = default;

	int i0;
};	

int
main(int argc, char *argv[])
{
	int a {10};
	int &r0 {a};
	int &r1 {r0};
	C c0;
	
	map<int, C&> m {
		{ 1, c0 }
	};

	cout << m[1].i0 << endl;
}
