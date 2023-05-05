#include <iostream>
#include <thread>
#include <chrono>
#include <sched.h>
#include <cstring>

using namespace std;
using namespace std::chrono_literals;

static void task_a()
{
	for (;;)
	{
		cerr << __func__ << " SCHEDULER: " << sched_getscheduler(0) << endl;;
		this_thread::sleep_for(1s);
	}
}

static void task_b()
{
	struct sched_param sp;
	memset(&sp, 0, sizeof(sp));
	sp.sched_priority = 50;
	sched_setscheduler(0, SCHED_FIFO, &sp);

	for (;;)
	{
		cerr << __func__ << " SCHEDULER: " << sched_getscheduler(0) << endl;;
		this_thread::sleep_for(1s);
	}
}

int main(int argc, char *argv[])
{
	thread th_a {task_a};
	thread th_b {task_b};

	struct sched_param sp;
	memset(&sp, 0, sizeof(sp));
	sp.sched_priority = 50;
	sched_setscheduler(0, SCHED_FIFO, &sp);
	
	for (;;)
	{
		cerr << __func__ << " SCHEDULER: " << sched_getscheduler(0) << endl;;
		this_thread::sleep_for(1s);
	}
}
