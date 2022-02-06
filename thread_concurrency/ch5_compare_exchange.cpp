#include <chrono>
#include <future>
#include <iostream>
#include <atomic>

int
main(int argc, char *argv[])
{
        std::atomic<bool> b;
        std::cout << __func__ << " is_lock_free() " << b.is_lock_free() << std::endl;
        b.store(true);
        {
                auto f = std::async(std::launch::async, [&] {
                                bool expected {false}, r;
                                std::cout << __func__ << "  expected: " << expected << " b: " << b.load() << std::endl;
                                while(!(r=b.compare_exchange_weak(expected, true)) && !expected) {
                                        std::cout << __func__ << "* expected: " << expected << " b: " << b.load() << std::endl;
                                        //expected = false;
                                }
                                std::cout << __func__ << r << " _ expected: " << expected << " b: " << b.load() << std::endl;
                        }
                        );

                std::this_thread::sleep_for(std::chrono::seconds(4));
        }
        b.store(true);

#if 0
        std::atomic<int> i;
        std::cout << __func__ << " is_lock_free() " << i.is_lock_free() << std::endl;
        i.store(1);
        auto f = std::async(std::launch::async, [&] {
                        std::cout << __func__ << " i: " << i.load() << std::endl;
                        int expected {2};
                        while(!i.compare_exchange_weak(expected, 3)) {
                                std::cout << __func__ << "* expected: " << expected << " i: " << i.load() << std::endl;
                        }
                        std::cout << __func__ << "_ expected: " << expected << " i: " << i.load() << std::endl;
                }
                );

        std::this_thread::sleep_for(std::chrono::seconds(4));
        i.store(2);
#endif
        return 0;
}
