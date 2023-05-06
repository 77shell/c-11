#include <memory>
#include <iostream>
#include <thread>
using namespace std;
void fun(std::shared_ptr<int> sp)
{
	std::cout << "in fun(): sp.use_count() == " << sp.use_count()
		  << " (object @ " << sp << ")\n";
	std::weak_ptr<int> wptr {sp};
	std::shared_ptr<int> sptr {wptr.lock()};
	std::cout << "in fun(): sp.use_count() == " << sp.use_count()
		  << " (object @ " << sp << ")\n";
	std::cout << "in fun(): wptr.use_count() == " << wptr.use_count() << std::endl;
}


void use_weakptr(std::weak_ptr<int> wp)
{
	cout << __func__ << endl;
	// Create a temporary ownership of sp1
	// sp1 can be deleted any time in main()
	// In here use_weakptr() creates a temporary ownership of sp1, even sp1 is deleted by main(),
	// it extends the original sp1's (i == 5) lifetime, until i(temporary shared_ptr is destoryed)
	std::shared_ptr<int> i {wp.lock()};
	if (i) {
		std::this_thread::sleep_for(3s);
		bool b {wp.expired()};
		// It won't be expired, since temporary std::shared_ptr<> hasn't destoryed
		// Until i is destoryed then expired() call will return true
		cout << "WP's ref obj is deleted " << b << endl;
		cout << "WP use_count() : " << wp.use_count() << endl;
		cout << "WP value : " << *i << endl;
		cout << "WP's address: " << i.get() << endl;
	}
}

int main()
{
	std::thread t;
	{
		auto sp1 = std::make_shared<int>(5);
		std::cout << "in main(): sp1.use_count() == " << sp1.use_count()
			  << " (object @ " << sp1 << ")\n";
 
		fun(sp1);

		cout << "sp1 use_count().a " << sp1.use_count() << endl;
		t  = std::move(thread {use_weakptr, std::weak_ptr<int> {sp1}});
		std::this_thread::sleep_for(100ms);
		cout << "sp1 use_count().b " << sp1.use_count() << endl;
		cout << "sp1's address: " << sp1.get() << endl;
		cout << "----------------------\n";
		sp1.reset(new int {6});
		cout << "sp1's value: " << *sp1 << endl;
		cout << "sp1 use_count().c " << sp1.use_count() << endl;
		cout << "sp1's address: " << sp1.get() << endl;
	}
	// cout << "New sp1 value " << *sp1 << endl;
	if (t.joinable())
		t.join();
}
