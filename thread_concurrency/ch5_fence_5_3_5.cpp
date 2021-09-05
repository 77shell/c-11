
#include <atomic>
#include <iostream>
#include <thread>
#include <assert.h>

using namespace std;

atomic<bool> x,y;
atomic<int> z;

void write_x_then_y()
{
	x.store(true, memory_order_relaxed);
	atomic_thread_fence(memory_order_release);
	y.store(true, memory_order_relaxed);
}


void write_y_then_x()
{
	while(!y.load(memory_order_relaxed));
	atomic_thread_fence(memory_order_acquire);
	if(x.load(memory_order_relaxed))
		++z;
}


int main(int argc, char *argv[])
{
	x = false;
	y = false;
	z = 0;
	thread a(write_x_then_y);
	thread b(write_y_then_x);
	a.join();
	b.join();
	assert(z.load() != 0);
}
