#include <thread>
#include <future>
#include <iostream>
#include <deque>
#include <mutex>

std::deque<std::promise<int>> data;
std::mutex mutex;


void store_incoming_data()
{
        std::unique_lock<std::mutex> lck {mutex, std::defer_lock};
        for(;;) {
                lck.lock();
                std::promise<int> p;
                data.push_back(std::move(p));
                std::cout << __func__ << std::endl;
                lck.unlock();
                std::this_thread::sleep_for(std::chrono::seconds(1));
        }
}


void process_incoming_data()
{
        std::unique_lock<std::mutex> lck {mutex, std::defer_lock};
        
        for(int i=0;; i++) {
                lck.lock();
                if(!data.empty())
                {
                        std::cout << __func__ << std::endl;
                        data.front().set_value(i);
                }
                lck.unlock();
                std::this_thread::sleep_for(std::chrono::seconds(1));
        }
}


int main(int argc, char *argv[])
{
        std::thread th_store {store_incoming_data};
        std::thread th_proc  {process_incoming_data};
        std::unique_lock<std::mutex> lck {mutex, std::defer_lock};
        
        for(;;)
        {
                lck.lock();
                if(!data.empty())
                {
                        std::cout << __func__ << std::endl;
                        std::promise<int> d {std::move(data.front())};
                        std::future<int> fu = d.get_future();
                        data.pop_front();
                        lck.unlock();
                        
                        std::cout << "Received data: " << fu.get() << std::endl;
                }
                else {
                        lck.unlock();
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        th_store.join();
        th_proc.join();
        return 0;
}
