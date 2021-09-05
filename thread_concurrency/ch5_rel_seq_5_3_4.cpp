#include <chrono>
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
using namespace std;

vector<int> queue_data;
std::atomic<int> cou;

static void process(int d, int id)
{
	cout << "ID: " << id << " " << __func__ << " " << d << endl;
}

static void wait_for_more_items(int id)
{
	cout << __func__ << id << endl;
	this_thread::sleep_for(chrono::milliseconds(2));
}

static void populate_queue()
{
	cout << __func__ << endl;
	unsigned const number_of_items = 20;
	queue_data.clear();
	for(unsigned i = 0; i<number_of_items; i++)
		queue_data.push_back(i);
	this_thread::sleep_for(chrono::seconds(3));
	cou.store(number_of_items, std::memory_order_release);
}

static void consume_queue_item(int id)
{
	while(true)
	{
		int item_index;
		if((item_index=cou.fetch_sub(1, std::memory_order_acquire)) <= 0)
		{
			wait_for_more_items(id);
			continue;
		}
		process(queue_data[item_index-1], id);
	}
}

int main(int argc, char *argv[])
{
	thread a(populate_queue);
	thread b(consume_queue_item, 'b');
	thread c(consume_queue_item, 'c');
	a.join();
	b.join();
	c.join();
}
