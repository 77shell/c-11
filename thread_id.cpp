#include <iostream>
#include <thread>
#include <chrono>
#include <future>

int main(int argc, char *argv[])
{
        std::thread::id i {std::thread::id()};
        std::thread::id j {std::this_thread::get_id()};
        std::cout << "Default thread id " << i << std::endl;
        std::cout << "Current thread id " << j << std::endl;

        std::promise<bool> p;
        auto future = p.get_future();

        std::thread t {
                [&p] {
                        while(1) {
                                std::this_thread::sleep_for(std::chrono::seconds {1});
                                std::cout << __func__ << std::endl;
                                p.set_value(true);
                                std::cout << __func__ << " set value\n";
                                break;
                        }
                }
        };

        while(1)
        {
                using namespace std::chrono_literals;
                auto status = future.wait_for(500ms);
                if(status == std::future_status::ready) {
                        std::cout << "Thread is finished\n";
                        break;
                }
                else {
                        std::cout << "Thread is still running\n";
                }
                
        }
        t.join();
}
