#include <atomic>
#include <iostream>
#include <future>

int
main(int argc, char *argv[])
{
	std::atomic<bool> b;
	bool x {b.load()};
	b.store(true);
	
	std::cout << "atmoic b: " << b.load() << std::endl;
	std::cout << "non-atmoic x: " << x << std::endl;
	std::cout << "exchange b & x\n";

	auto f = std::async(std::launch::async, [&]{
			x = b.exchange(false);
			std::cout << "atmoic b: " << b.load() << std::endl;
			std::cout << "non-atmoic x: " << x << std::endl;
		}
		);
	f.get();
	
	return 0;
}
