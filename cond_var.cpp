#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <chrono>

using namespace std;

mutex m;
condition_variable cv;
int val;

void send_event()
{
	cout << __func__ << endl;
	
	for(val=0; val<11;)
	{
		{
			lock_guard<mutex> lck {m};
			val++;
			cout << __func__ << " Change val to " << val << endl;
		}
		if(val != 1) {
			cout << __func__ << " cv.notify()\n";
			cv.notify_all();
		}
		/*
		 * This delay is necessary, otherwise threads which are waiting the event
		 * cannot be woken up.
		 */
		this_thread::sleep_for(chrono::milliseconds {10});
	}

	cout << __func__ << " End" << endl;
}

void receive_event(const char *name, int desire)
{
	cout << "\t" << name << " Start\n";
	val = 0;
	unique_lock<mutex> lck {m};
	cout << "\t" << name << __func__ << " (Locked)\n";
	cv.wait(lck, [&]{
			     cout << "\t" << name << " Waiting (" << val << ")\n";
			     return val == desire;
		     }
		);
	cout << "\t" << name << __func__ << " (got event)\n";
}

int main(int argc, char *argv[])
{
	//SLAVE slave;
	thread thread_rx1 {receive_event, "> th2", 1};
	this_thread::sleep_for(chrono::seconds {1});
	thread thread_rx2 {receive_event, "+ th3", 3};
	this_thread::sleep_for(chrono::seconds {2});
	thread thread_tx {send_event};
	thread_tx.join();
	thread_rx1.join();
	thread_rx2.join();
	return 0;
}
