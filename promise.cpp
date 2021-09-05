#include <thread>
#include <iostream>
#include <future>

using namespace std;

int
main(int argc, char *argv[])
{
	// thread {[] {
	// 	while(1) {
	// 		this_thread::sleep_for(chrono::seconds {1});
	// 		cout << "__ ";
	// 	}
	// }
	// };
	promise<void> p;
	shared_future<void> f {p.get_future().share()};
	thread t {[&, f] { // capture references except f (f is copied by value)
		cout << "Thread starts\n";
		f.get();
		cout << "Thread ends\n";
	}
	};

	int i {5};
	while(i--) {
		this_thread::sleep_for(chrono::seconds {1});
		cout << "W \n";
		fflush(stdout);
	}
	p.set_value();

	this_thread::sleep_for(chrono::seconds {1});
	t.join();
	return 0;
}
