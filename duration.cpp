#include <iostream>
#include <thread>
#include "/home/max/src/csu.linux/include/util/duration.h"

int
main(int argc, char *argv[])
{
	using namespace std::chrono_literals;
	util::DURATION<std::chrono::seconds> t;
	for(;;)
	{
		std::cout << t.lasting_time() << std::endl;
		std::this_thread::sleep_for(1s);
		std::chrono::seconds k {t.elapsed()};
		std::cout << k.count() << std::endl;
	}
}
