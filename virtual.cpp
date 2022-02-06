#include <iostream>

using namespace std;

class A {
public:
	virtual void a() = 0;
};


class B : public A {
public:
	static B& getInstance() {
		static B b;
		return b;
	}
	
	virtual void a() {
		std::cout << __func__ << std::endl;
	}
	
private:
	B() {}
};

class C : public A {
public:
	virtual void a() { cout << "C " << __func__ << endl; }
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
	// B::getInstance().a();
	C *c = new C;
	S s {c};
	s.pA->a();
	return 0;
}
