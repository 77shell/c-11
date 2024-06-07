
#include <iostream>
#include <memory>
#include <queue>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono_literals;

struct S {
	S() {
		cout << __func__ << " default ctor " << s << endl;
	}

	S(int i)
		: s {i}
		{
			cout << __func__ << " ctor(int i) " << s << endl;
		}

	~S() {
		cout << __func__ << " " << s << endl;
	}

	int s {1};
};

std::unique_ptr<S>
create_s()
{
	return make_unique<S>(99);
}

int
main(int argc, char *argv[])
{
	queue<unique_ptr<S>> qu;
	qu.push(make_unique<S>(10));
	qu.push(std::move(create_s()));

	int r = getchar();
	unique_ptr<S> p0;
	p0 = std::move(qu.front());
	cout << p0->s << endl;
	cout << qu.front().get() << endl;
	cout << "------------------\n";
	this_thread::sleep_for(1s);
}
