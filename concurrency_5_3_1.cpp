
#include <iostream>
#include <vector>
#include <thread>

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
		cout << "X's ctor()\n";
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
	cout << t << " ";
}


template<class T>
void call_f(vector<T> &v) noexcept(noexcept(f(v[0])))
{
	for(auto x:v) {
		f(x);
		cout << std::boolalpha << noexcept(f(v[0])) << endl;
	}
}


void thread_1()
{
	cout << __func__ << endl;
	vector<int> v { 1, 2, 3, 4 };
	int i {100};
  
	while(i--)
	{
		call_f(v);
		cout << "Thread-1: counting down : " << i << endl;
		std::chrono::seconds wait_2sec(2);
		this_thread::sleep_for(wait_2sec);
	}
}


void thread_2()
{
	cout << __func__ << endl;
	vector<char> c ({ 'a', 'b', 'c', 'd' });
	cout << "vector::c's size: " << c.size() << endl;
	initializer_list<int> l{1, 2, 3};
	int i {100};
  
	while(i--)
	{
		for(auto i:l)
		{
			cout << i << "-time" << endl;
			call_f(c);
		}
		std::chrono::seconds wait_2sec(2);
		this_thread::sleep_for(wait_2sec);
		cout << "Counting down: " << i << endl;
	}
}


int main(int argc, char *argv[])
{
	thread t1{thread_1};
	thread t2{thread_2};

	t1.join();
	t2.join();

	X x(100, 100);

	terminate_handler old = term_handler;
	set_terminate(old);
  
	return 0;
}
