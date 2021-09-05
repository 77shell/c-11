
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <atomic>
#include <cassert>


struct X {
	int i;
	std::string s;
};

std::atomic<X*> p;
std::atomic<int>a;

void use_x()
{
	X* x;
	while(!(x=p.load(std::memory_order_consume)))
	{
		std::cout << __func__ << " waiting p to be ready..." << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	std::cout << __func__ << " load x with p successful" << std::endl;
	assert(x->i == 42);
	assert(x->s == "hello");
	assert(a.load(std::memory_order_relaxed) == 99);
}

void create_x()
{
	std::cout << __func__ << std::endl;
	
	X *x = new X;
	x->i = 42;
	x->s = "hello";

	std::cout << __func__ << " (sleep 3 sec) before updating x to p" << std::endl;
	
	std::this_thread::sleep_for(std::chrono::seconds(3));
	a.store(99, std::memory_order_relaxed);
	std::cout << __func__ << " load p with x" << std::endl;
	p.store(x,  std::memory_order_release);
}

int main(int argc, char *argv[])
{
	std::thread t1 {create_x};
	std::thread t2 {use_x};
	t1.join();
	t2.join();
	
	return 0;
}
