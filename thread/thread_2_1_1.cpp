#include <iostream>
#include <thread>
#include <future>
#include <chrono>

using namespace std;

class background_task {
public:
	background_task(int label)
		: name {label}
		{
			cout << __func__ << endl;
		}

	void operator()() {
		do_something();
	}

	int name;

private:
	void do_something() {
		while (1) {
			cout << "Name: " << name << endl;
			cout << "Hello" << endl;
			cout << __func__ << endl;
			this_thread::sleep_for(3s);
		}
	}
};


int main(int argc, char *argv[])
{
	uint16_t i {101};
	float f {i / 10.0f};
	std::cout << f << std::endl;
	thread my_thread;
	{
		thread th1;
		//thread my_thread2 ([] { background_task a; a(); });
		future<int> f;
		promise<int> p;
		th1 = thread {background_task {1}};
		this_thread::sleep_for(1s);
		// pthread_cancel(th1.native_handle());
		if (th1.joinable()) {
			cout << "Joining thread-1 ..\n";
			// th1.join();
		}
	}
	for (;;) {
		cout << "Start 2nd thread\n";
		this_thread::sleep_for(1s);
	}
	my_thread = thread {background_task {2}};
	if (my_thread.joinable()) {
		cout << "Joining thread-2 ..\n";
		my_thread.join();
	}

	return 0;
}
