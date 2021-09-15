
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <math.h>

using namespace std;

template <class exe>
class C {
public:
	C(exe e) : E{e}
		{}
	
	exe E;
	virtual void execute() { E(9); }
};


// Lamdba
//C<[](int i){std::cout << i << std::endl;}> c;

// pointer to function
using PF = double (*)(double);
// A lamdba is assigned to a 'pointer-to-function'
double (*pf)(double) = [](double a){ std::cout << sqrt(a) << std::endl; return sqrt(a); };
//using PF = pf;
C<PF> cc(pf);


struct S {
	S(initializer_list<int> a) {
		cout << "initializer_list" << endl;
		for(auto i:a)
			cout << i;
		cout << endl;
		for(auto p=a.begin(); p!=a.end(); ++p)
			cout << *p;
		cout << endl;
	}
	
	S() {
		cout << "Default constructor" << endl;
	}
	
	S(int i) {
		cout << "S(int) ctor" << endl;
	}
	
	int x;
	string s;
};


int main(int argc, char *argv[])
{
	char *q{new char[1024]{}};
	//S s1 = { 1, "hello" };
	S s2 { 1, 2, 3, 4, 5 };
	S s3 {};
	S s4;

	cc.execute();

	char c = alignof(char);
	cout << "size of long : " << sizeof(1L) << "-bytes" << endl;
	cout << "size of long long : " << sizeof(1LL) << "-bytes" << endl;
	cout << "size of int : " << sizeof(int) << "-bytes" << endl;
	cout << "size of short : " << sizeof(short) << "-bytes" << endl;
	static_assert(sizeof(short) == 2, "unexpected short-size");
	cout << "size of size_t : " << sizeof(size_t) << "-bytes" << endl;

	alignas(int) char buffer[1024];
}


void try_vector()
{
	vector<int> v { 1, 2, 3, 4 };
	for(auto x : v) {
		cout << x << endl;
	}
  
	for(auto &x : v) {
		x += 1;
	}

	for(auto x : v) {
		cout << x << endl;
	}
}
