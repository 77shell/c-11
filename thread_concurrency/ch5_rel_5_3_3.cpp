/*
 * C++ Concurrency in Action
 * Chapter 5
 * RELAXED ORDERING
 * Page 150
 *
 * 2020 June 7
 */

#include <iostream>
#include <atomic>
#include <thread>
#include <assert.h>

using namespace std;

std::atomic<bool> x,y;
std::atomic<int> z;

void write_x_then_y()
{
	x.store(true, memory_order_relaxed);
	y.store(true, memory_order_relaxed);
}


void write_y_then_x()
{
	while(!y.load(memory_order_relaxed));
	if(x.load(memory_order_relaxed))
		++z;
}


int main(int argc, char *argv[])
{
	x = false;
	y = false;
	z = 0;
	thread b(write_y_then_x);
	thread a(write_x_then_y);
	a.join();
	b.join();
        cout << "z " << z << endl;
	assert(z.load() != 0);
}
