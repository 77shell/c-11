#include <chrono>
#include <future>
#include <iostream>
#include <atomic>

int
main(int argc, char *argv[])
{
        std::atomic_flag atm_f;
        auto f = std::async(std::launch::async, [&] {
                        while(atm_f.test_and_set());
                        std::cout << __func__ << " enter atm_f test and set\n";
                        std::this_thread::sleep_for(std::chrono::seconds(2));
                        atm_f.clear();
                        std::cout << __func__ << " leave atm_f test and set\n";
                }
                );

        while(atm_f.test_and_set());
        std::cout << "Lock free type ? : " << ATOMIC_BOOL_LOCK_FREE << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        atm_f.clear();
        return 0;
}
