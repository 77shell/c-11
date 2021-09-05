
#include <iostream>
#include <vector>

using namespace std;


class X {
	vector<int> vi;
	vector<string> vs;
  
public:
	X(int, int);
};


X::X(int sz1, int sz2)
try
	: vi(sz1), vs(sz2)
	{
	}
catch(std::exception &e) {
	cout << "class X, std::exception: " << endl;
}


using terminate_handler = void(*)();
[[noreturn]] void term_handler()
{
	cout << "term_handler" << endl;
  
}


template<class T>
void f(T t)
{
	cout << t;
}


template<class T>
void call_f(vector<T> &v) noexcept(noexcept(f(v[0])))
{
	for(auto x:v) {
		f(x);
		cout << std::boolalpha << noexcept(f(v[0])) << endl;
	}
}


int main(int argc, char *argv[])
{
	vector<int> v { 1, 2, 3, 4 };
	call_f(v);

	vector<char> c { 'a', 'b', 'c', 'd' };
	call_f(c);

	X x(100, 100);

	terminate_handler old = term_handler;
	set_terminate(old);
  
	return 0;
}

