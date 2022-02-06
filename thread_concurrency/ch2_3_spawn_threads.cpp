
/*
 * spawns threads
 */

#include <thread>
#include <vector>
#include <iostream>

class background_task {
public:
        void operator()(int i) {
                do_something(i);
        }

private:
        void do_something(int i) {
                std::cout << __func__ << i << std::endl;
        }
};


class scope_thread {
        std::thread t;
public:
        scope_thread(std::thread t_)
                : t {std::move(t_)}
                {
                }

        ~scope_thread()
                {
                        if(t.joinable())
                                t.join();
                }
};


int main(int argc, char *argv[])
{
#if 0
        std::vector<std::thread> threads;
        for(int i=0; i<20; i++)
        {
                threads.emplace_back(background_task(), i);
        }

        for(auto &entry : threads)
                entry.join();
#endif

        scope_thread st {std::thread {background_task(), 10}};
#if 0
        std::vector<scope_thread> scope_threads {
                scope_thread (std::thread (background_task(), 1)),
                        scope_thread (std::thread (background_task(), 2)),
                        scope_thread (std::thread (background_task(), 3))
                        };
#endif
        std::vector<scope_thread> scope_threads;
        scope_threads.emplace_back(std::thread(background_task(), 1));
        return 0;
}
