#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

std::vector<int> queue_data;
std::atomic<int> count;
using namespace std;

void wait_for_more_items(std::string msg) {
        cout << msg << "\t" << __func__ << endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
}

void process(int data, std::string name) {
        cout << name << "-" << __func__ << ": " << data << endl;
}

void populate_queue()
{
        unsigned const number_of_items=20;
        queue_data.clear();
        for(unsigned i=0;i<number_of_items;++i)
        {
                queue_data.push_back(i);
        }
        cout << "count.store()\n" << endl;
        this_thread::sleep_for(chrono::milliseconds {200});
        count.store(number_of_items,std::memory_order_release); // (1) Initial store()
}

void consume_queue_items(std::string name, chrono::milliseconds t)
{
        while(true)
        {
                int item_index;
                if((item_index=count.fetch_sub(1, std::memory_order_acquire))<=0) // (2) an RMW opr
                {
                        wait_for_more_items(name); // (3) wait for more items
                        continue;
                }
                //this_thread::sleep_for(t);
                process(queue_data[item_index-1], name); // (4) reading queue_data is safe
                //std::this_thread::yield();
        }
}

int main()
{
        std::thread a {populate_queue};
        std::thread b {consume_queue_items, "thread-w", chrono::milliseconds {10}};
        std::thread c {consume_queue_items, "thread-x", chrono::milliseconds {10}};
        std::thread d {consume_queue_items, "thread-y", chrono::milliseconds {10}};
        std::thread e {consume_queue_items, "thread-z", chrono::milliseconds {10}};
        
        a.join();
        b.join();
        c.join();
        d.join();
}
