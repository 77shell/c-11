
#include <thread>
#include <chrono>
#include <iostream>


int
main(int argc, char *argv[])
{
        auto t0 = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << "ms\n";
}
