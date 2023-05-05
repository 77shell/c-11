#include <iostream>
#include <thread>
#include <future>
#include <chrono>

using namespace std;
using namespace chrono_literals;

struct A {
	A(string s) : name {s} {
		cout << __func__ << name << endl;
	}

	~A() {
		cout << __func__ << name << endl;
	}

	string name;
};

class background_task {
public:
	background_task(int label)
		: name {label}
		{
			cout << __func__ << endl;
		}

	void operator()()
		{
			do_something();
		}

	int name;

private:
	void do_something() {
		for (;;)
		{
			cout << __func__ << " start\n";
			{
				cout << __func__ << " 1st section\n";
				A a {"a1"};
				this_thread::sleep_for(1s);

				A a2 {"a1.1"};
				this_thread::sleep_for(1s);
			}
			{
				cout << __func__ << " 2nd section\n";
				A a {"a2"};
				this_thread::sleep_for(1s);

				A a2 {"a2.1"};
				this_thread::sleep_for(1s);

			}
		}
	}
};


struct S {
	S()
		: task {999}
		{
			th = thread {task};
			tid = th.native_handle();
		}

	~S() {
		cout << __func__ << " cancel thread\n";
		pthread_cancel(tid);
		if (th.joinable())
			th.join();
	}

	background_task task;
	thread th;
	pthread_t tid;
};

int main(int argc, char *argv[])
{
	{
		S S1 {};
		this_thread::sleep_for(500ms);
	}

	{
		S S2 {};
		this_thread::sleep_for(500ms);
	}

	this_thread::sleep_for(3s);
	return 0;
}
