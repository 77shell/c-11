
#include <thread>
#include <chrono>
#include <iostream>


int
main(int argc, char *argv[])
{
        using namespace std::chrono_literals;
        auto t0 = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(1s);
        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << "ms\n";

        std::chrono::time_point<std::chrono::steady_clock> p0 {std::chrono::steady_clock::now()};
        std::this_thread::sleep_for(1s);
        std::chrono::time_point<std::chrono::steady_clock> p1 {std::chrono::steady_clock::now()};
        std::cout << "Time elapse: " << std::chrono::duration_cast<std::chrono::milliseconds>(p1-p0).count() << "ms\n";

	std::chrono::milliseconds s {3s};
	std::chrono::milliseconds p {50ms};
	std::chrono::milliseconds ms;

	std::cout << "Count: " << ms.count() << "count: " << s.count() / p.count() << std::endl;;
}
