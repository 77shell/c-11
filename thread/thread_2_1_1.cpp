#include <iostream>
#include <thread>
#include <future>

class background_task {
public:
	background_task() { std::cout << __func__ << std::endl; }
	void operator()() {
		do_something();
	}

private:
	void do_something() {
		std::cout << "Hello" << std::endl;
		std::cout << __func__ << std::endl;
	}
};


int main(int argc, char *argv[])
{
	std::thread my_thread (background_task());
	//std::thread my_thread2 ([] { background_task a; a(); });
	std::future<int> f;
	std::promise<int> p;

	return 0;
}
