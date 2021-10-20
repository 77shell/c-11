
#include <future>
#include <iostream>
using namespace std::literals;
std::promise<void> p;

int
main(int argc, char *argv[])
{
	auto sf = p.get_future().share();

	/* 
	 * Without std::future to receive async()'s return, 
	 * program will block the call to async()
	 *
	 */
	std::future<int> h = std::async(std::launch::async, [sf] {
								    for(;;) {
									    std::future_status s {sf.wait_for(500ms)};
									    switch(s) {
									    case std::future_status::ready: std::cout << " ready\n"; return 1;
									    case std::future_status::timeout: std::cout << " timeout\n"; break;
									    case std::future_status::deferred: std::cout << " deferred\n"; break;
									    }
								    }
								    // sf.wait();
								    std::cout << "got event!\n";
								    return 1;
							    }
		);

	for(int i=0; i<3; i++)
	{
		std::this_thread::sleep_for(std::chrono::seconds {1});
		std::cout << i << std::endl;
	}

	p.set_value();
	std::this_thread::sleep_for(std::chrono::milliseconds {100});
	std::cout << "async return " << h.get() << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds {1});
	return 0;							
}
