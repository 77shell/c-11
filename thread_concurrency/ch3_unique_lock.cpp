
#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>

std::mutex global_mutex;
int global_i {0};

/*
 * Demonstrate the ownership transferring on unique_lock<> 
 * (unique_lock can be returned)
 *
 */

std::unique_lock<std::mutex>
gateway()
{
        std::unique_lock<std::mutex> lk(global_mutex);
        global_i++;
        std::cout << __func__ << " :Global variable: " << global_i << std::endl;
        return lk;
}


void
task1()
{
        std::unique_lock<std::mutex> lk {gateway()};
        std::this_thread::sleep_for(std::chrono::seconds(2));
        global_i++;
        std::cout << __func__ << " :Global variable: " << global_i << std::endl;
}


void
task2()
{
        std::unique_lock<std::mutex> lk {gateway()};
        global_i++;
        std::cout << __func__ << " :Global variable: " << global_i << std::endl;
}


int
main(int argc, char *argv[])
{
        std::thread th1 {task1};
        std::thread th2 {task2};
        
        th1.join();
        th2.join();
        return 0;
}

