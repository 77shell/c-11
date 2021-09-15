#include <iostream>


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


int
main(int argc, char *argv[])
{
	std::cout << "Hello" << std::endl;
	B::getInstance().a();
	return 0;
}
