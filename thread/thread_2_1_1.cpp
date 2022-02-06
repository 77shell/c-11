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
		cout << "Name: " << name << endl;
		cout << "Hello" << endl;
		cout << __func__ << endl;
		this_thread::sleep_for(3s);
	}
};


int main(int argc, char *argv[])
{
	thread my_thread;
	//thread my_thread2 ([] { background_task a; a(); });
	future<int> f;
	promise<int> p;
	my_thread = thread {background_task {1}};
	if (my_thread.joinable()) {
		cout << "Joining thread-1 ..\n";
		my_thread.join();
	}

	my_thread = thread {background_task {2}};
	if (my_thread.joinable()) {
		cout << "Joining thread-2 ..\n";
		my_thread.join();
	}

	return 0;
}
