
#include <iostream>


using namespace std;

class A {
public:
	A(int a)
		: name(a)
		{}
	int name;		
};

struct S {
	int s;
};

class E {
public:
	E()
		try
			: a(0), s{10}
			  {
			  }
	catch(std::exception) {
		cout << "std::exception" << endl;
	}
	
	A a;
	S s;
	virtual void do_something(void) { std::cout << "E: Do something" << a.name << s.s << std:: endl; }
};


class F : public E {
public:
	F() {}
	virtual void do_something(void) { std::cout << "F: Do something" << std::endl; }
};
   

void f()
{
	std::cout << "Hello" << std::endl;
	throw F{};
}


int main(int argc, char *argv[])
	try {
		f();
		exit(EXIT_SUCCESS);
	}
	catch(E obj) {
		obj.do_something();
	}
	catch(E) {
		cout << "Catch you" << endl;
		exit(EXIT_SUCCESS);
	}
	catch(F obj) {
		obj.do_something();
	}
	catch(...) {
		cout << "Catch everything" << endl;
	}

