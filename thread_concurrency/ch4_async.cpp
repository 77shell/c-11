#include <future>
#include <iostream>
#include <thread>


/*
 * defer execuation till get() is called
 * std::async(std::launch::deferred, func)
 */
int deferred_background_task()
{
        int i {0};
        for(;i < 5;) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << __func__ << i << std::endl;
                i++;
        }
        return i;
}

class FuncObject {
public:
        int operator()(int a) {
                for(; a<5; a++) {
                        std::cout << __func__ << a << std::endl;
                        std::this_thread::sleep_for(std::chrono::milliseconds(300));
                }
                return a;
        }

};

/*
 * Start executing immediately 
 * std::async(std::launch::async, func)
 */
int async_background_task()
{
        int i {0};
        for(;i < 5;) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << __func__ << i << std::endl;
                i++;
        }
        return i;
}


int foreground_task()
{
        int i {0};
        for(;i < 5;) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::cout << __func__ << i << std::endl;
                i++;
        }
        return i;
}


int main(int argc, char *argv[])
{
        std::future<int> feedback_deferred {std::async(std::launch::deferred, deferred_background_task)};
        std::future<int> feedback_async    {std::async(std::launch::async, async_background_task)};
        std::future<int> feedback_async2   {std::async(std::launch::async, FuncObject(), 2)};
        foreground_task();
        
        std::cout << "Feedback_async:\t"    << feedback_async.get() << std::endl;
        std::cout << "Feedback_async2:\t"   << feedback_async2.get() << std::endl;
        std::cout << "Feedback_deferred:\t" << feedback_deferred.get() << std::endl;
        return 0;
}
