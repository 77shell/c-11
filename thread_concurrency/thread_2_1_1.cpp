
/*
 * backgroup_task is a function object
 * transfer rvalue by std::ref()
 */

#include <thread>
#include <iostream>

class background_task {
public:
        void operator()() {
                do_something();
        }

private:
        void do_something() {
                std::cout << __func__ << std::endl;
        }
};


static void thread_task(int &i)
{
        std::cout << __func__ << i << std::endl;
}


int main(int argc, char *argv[])
{
        int local_i {1};
        background_task func_obj;
        std::thread
                th1 {func_obj}, // function object
                th2 {thread_task, std::ref(local_i)};
        
        //th.detach();
        th1.join();
        th2.join();

        unsigned int const &ref {std::thread::hardware_concurrency()};
        std::cout << "hardware_concurrency : " << ref << std::endl;

        return 0;
}
