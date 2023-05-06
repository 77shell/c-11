#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono_literals;

class A {
public:
	A() {
		ctor_task();
	}

	virtual void a()
		{ cout << "A " << __func__ << endl; }

	virtual void ctor_task()
		{ cout << "A " << __func__ << endl; }
};


class B : public A {
public:
	virtual void a()
		{ std::cout << "B " << __func__ << std::endl; }

	virtual void ctor_task()
		{ cout << "B " << __func__ << endl; }
	
	B() {
		ctor_task();
	}
};

class C : public A {
public:
	virtual void a()
		{ cout << "C " << __func__ << endl; }
};

struct S {
	S(A *p)
		: pA {p}
		{}

	A *pA;
};


int
main(int argc, char *argv[])
{
	std::cout << "Hello" << std::endl;
	B *b {new B {}};
	std::this_thread::sleep_for(2s);
	return 0;
}
