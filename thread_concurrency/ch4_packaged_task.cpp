#include <chrono>
#include <deque>
#include <mutex>
#include <future>
#include <thread>
#include <vector>
#include <iostream>


int inc_task(int start_idx, int count)
{
        int end {start_idx + count};
        for(; start_idx<=end; start_idx++) {
                std::cout << __func__ << start_idx << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        return start_idx;
}

std::deque<std::packaged_task<int(int,int)>> tasks;


std::future<int> post_a_job()
{
        std::cout << __func__ << std::endl;
        std::packaged_task<int(int,int)> job {inc_task};
        std::future<int> f {job.get_future()};
        tasks.push_back(std::move(job));
        return f;
        
}


void create_jobs()
{
        for(;;) {
                std::future<int> f1 = post_a_job();
                /*
                 * the call to f1.get() will block until f1 be ready.
                 */
                std::cout << __func__ << f1.get() << std::endl;
                
                std::future<int> f2 = post_a_job();
                std::cout << __func__ << f2.get() << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(10));
        }
}


int main(int argc, char *argv[])
{
        std::thread th {create_jobs};

        for(;;)
        {
                while(!tasks.empty())
                {
                        std::packaged_task<int(int,int)> job {std::move(tasks.front())};
                        tasks.pop_front();
                        std::cout << "Start a job\n";
                        /*
                         * When job() is finished, the associated future will then be made ready
                         */
                        job(2, 3);
                        std::cout << "End a job\n";
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        return 0;
}
