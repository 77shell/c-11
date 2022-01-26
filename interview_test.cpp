#include <iostream>
using namespace std;

int recursive(int n)
{
	cout << __func__ << "\t" << n << endl;
	return n <= 1
		? 1
		: n * recursive(n - 1);
}

int main(int argc, char *argv[])
{
	if(argc == 1) {
		cout << "Input a start number for recursive calculation\n";
		return 0;
	}
	cout << recursive(atoi(argv[1])) << endl;
	return 0;
}
