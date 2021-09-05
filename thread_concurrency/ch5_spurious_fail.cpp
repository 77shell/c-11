
#include <chrono>
#include <atomic>
#include <iostream>
#include <future>

int
main(int argc, char *argv[])
{
#if 0
	std::atomic<bool> b;
	auto f2 = async(std::launch::async, [&]{
			std::cout << __func__ << std::endl;
			for(;;) {
				long ok, ng;
				for(bool expected=false;;expected=false) {
					std::cout << "> expected: " << expected << " atomic-var: " << b.load() << std::endl;
					bool r;
                                        while(!(r=b.compare_exchange_weak(expected, true)))
						std::cout << "? no change on atomic-var,  expected : " << expected << " r: " << r << std::endl;
                                        std::cout << "< expected : " << expected << " atomic-var: " << b.load() << " r: " << r << std::endl << std::endl;;
					      //&& !expected)
					//expected ? ok++ : ng++;
					//std::cout << "OK " << ok << " NG " << ng << std::endl;
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
			}
		}
		);
#endif

#if 1
	std::atomic<int> i;
	auto f2 = async(std::launch::async, [&]{
			std::cout << __func__ << std::endl;
			for(;;) {
				long ok, ng;
				for(int expected=88;;expected=88) {
					std::cout << "> expected: " << expected << " atomic-var: " << i.load() << std::endl;
					bool r;
					while(!(r=i.compare_exchange_weak(expected, 66)))
						std::cout << "? no change on atomic-var,  expected : " << expected << std::endl;
					expected == 88 ? ok++ : ng++;
					std::cout << "< expected : " << expected << " atomic-var: " << i.load() << " r: " << r << std::endl << std::endl;;
					//std::cout << "OK " << ok << " NG " << ng << std::endl;
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
			}
		}
		);
#endif

	std::this_thread::sleep_for(std::chrono::seconds(4));

#if 0
	auto f1 = std::async(std::launch::async, [&]{
			for(;;) {
				b.store(true);
//std::cout << "b : " << b.load() << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
		);
#endif

#if 0
	std::cout << __func__ << " wake up\n";
	auto f1 = std::async(std::launch::async, [&] {
			for(;;) {
				i.store(88);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
		);
#endif
	return 0;
}
