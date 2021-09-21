#include <iostream>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <future>

template<class T>
struct queue_t {
	template<class Rep, class Period>
	bool try_pop(T &data, std::chrono::duration<Rep,Period> &&wait)
		{
			std::unique_lock<std::mutex> lck {mtx};
			if(qu.size() > 0) {
				data = qu.front();
				qu.pop_front();
				return true;
			}
			return cv.wait_for(lck, wait,
					   [&]{
						   std::cerr << ",";
						   if(qu.empty())
							   return false;
						   data = qu.front();
						   qu.pop_front();
						   return true;
					   }
				);
		}

	T pop()
		{
			std::unique_lock<std::mutex> lck {mtx};
			T r;
			cv.wait(lck, [&]{
					     std::cerr << ".";
					     if(qu.empty())
						     return false;
					     r = qu.front();
					     qu.pop_front();
					     return true;
				     }
				);
			return r;
		}

	void push(T &data)
		{
			bool notify;
			{
				std::lock_guard<std::mutex> lck {mtx};
				notify = qu.empty();
				qu.push_back(data);
			}
			if(notify) {
				cv.notify_all();
				std::this_thread::yield();
			}
		}

private:
	std::deque<T> qu;
	std::mutex mtx;
	std::condition_variable cv;
};

queue_t<int> Q;
using namespace std::chrono_literals;

void
W()
{
	int in;
	for(int i=0;;i++) {
		//std::cin >> in;
		Q.push(i);
		std::cin.clear();
		std::this_thread::sleep_for(1s);
	}
}

void
R1()
{
	for(;;)
	{
		int i {Q.pop()};
		std::cerr << __func__ << "\t" << i << std::endl;
	}
}

void
R2()
{
	int i;
	for(;;)
	{
		if(Q.try_pop(i, 300ms))
			std::cerr << __func__ << "\t" << i;
		std::cerr << std::endl;
		//else
		//	std::cerr << __func__ << "\t" << "pop fail" << std::endl;
	}
}

int main(int argc, char *argv[])
{
	auto t1 {std::async(std::launch::async, &W)};
	auto t2 {std::async(std::launch::async, &R1)};
	auto t3 {std::async(std::launch::async, &R2)};
}
