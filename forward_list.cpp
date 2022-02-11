#include <forward_list>
#include <iterator>
#include <iostream>
#include <mutex>
#include <thread>
#include <future>
#include <condition_variable>

using namespace std;

struct IMBUSdev {
	IMBUSdev(uint8_t ad) : _state {eDisconnected}, modaddr {ad}
		{}

	~IMBUSdev()
		{}

	enum State_e {
		eError,
		eDisconnected,
		eConnecting,
		ePreparing,
		ePrepared,
		eOperational,
		eSWdownloading
	} _state;

	State_e state() const {
		return _state;
	}
	uint8_t modaddr;
};

template <class T>
struct ptr_list {

	struct ptr_deleter {
		void operator()(T *dev) const {
			std::cout << "Call delete from function object. dev->" << static_cast<int>(dev->modaddr) << '\n';
		}
	};

	template <class U>
	struct ptr : public std::shared_ptr<U> {
		ptr(U *src, ptr_deleter d, ptr_list *p)
			: std::shared_ptr<U> {src, d}, ptrlist {p}
			{}

		ptr(ptr_list *p)
			: ptrlist {p}
			{
				cout << __func__ << p << endl;
			}

		~ptr() {
			cout << __func__ << ptrlist << endl;
			// std::lock_guard<std::mutex> lck {ptrlist->list_lock};
			ptrlist->cv.notify_one();
		}

		ptr_list *ptrlist;
	};

	void push(T *dev) {
		std::lock_guard<std::mutex> lck {list_lock};
		for (auto p : list)
			if (p.get() == dev)
				return;

		list.push_front(ptr<T>(dev, ptr_deleter {}, this));
		listchanged_flag.clear();
	}

	void remove(T *dev) {
		std::unique_lock<std::mutex> lck {list_lock};
		cout << __func__ << endl;
		list.remove_if(
			[&](ptr<T> &p) {
				if (p.get() == dev) {
					cv.wait(lck, [&]() {
							     cout << "*** " << __func__ << ": p use count " << p.use_count() << endl;
							     return p.use_count() <= 1;
						     }
						);
					return true;
				}
				return false;
			}
			);
		listchanged_flag.clear();
		cout << __func__ << ": end\n";
	}

	void pop_front() {
		std::lock_guard<std::mutex> lck {list_lock};
		if (!list.empty()) {
			cout << __func__ << endl;
			list.pop_front();
			listchanged_flag.clear();
		}
	}

	void clear() {
		std::lock_guard<std::mutex> lck {list_lock};
		if (!list.empty()) {
			list.clear();
			listchanged_flag.clear();
		}
	}

	bool list_changed() {
		return !listchanged_flag.test_and_set();
	}

	struct iterator {
		iterator(ptr_list &v)
			: dev {v}
			{
				std::lock_guard<std::mutex> lck {dev.list_lock};
				list_iterator = dev.list.begin();
			}

		bool operator!=(const typename std::forward_list<ptr<T>>::iterator &rhs) {
			std::lock_guard<std::mutex> lck {dev.list_lock};
			return list_iterator != rhs;
		}

		bool operator==(const typename std::forward_list<ptr<T>>::iterator &rhs) {
			std::lock_guard<std::mutex> lck {dev.list_lock};
			return list_iterator == rhs;
		}

		ptr<T> operator*() {
			std::lock_guard<std::mutex> lck {dev.list_lock};
			// if (dev.list_changed())
			// 	list_iterator = dev.list.begin();
			return *list_iterator;
		}

		ptr<T> operator++(int) { // Post-increment, iterator++
			std::lock_guard<std::mutex> lck {dev.list_lock};
			// if (dev.list_changed()) {
			// 	list_iterator = dev.list.begin();
			// 	return *list_iterator;
			// }

			return list_iterator == dev.list.end()
				? nullptr
				: *list_iterator++;
		}

	private:
		ptr_list &dev;
		typename std::forward_list<ptr<T>>::iterator list_iterator;
	};

	typename std::forward_list<ptr<T>>::iterator begin() {
		std::lock_guard<std::mutex> lck {list_lock};
		return list.begin();
	}

	typename std::forward_list<ptr<T>>::iterator end() {
		std::lock_guard<std::mutex> lck {list_lock};
		return list.end();
	}

	std::atomic_flag listchanged_flag;
	std::mutex list_lock;
	std::condition_variable cv;
	std::forward_list<ptr<T>> list;
};

ptr_list<IMBUSdev> mDevList;

std::forward_list<int> X;
std::mutex mutex_terminate;
std::mutex mutex_signal;

void
do_work(promise<void> p)
{
	using namespace std::chrono_literals;
	cout << __func__ << endl;
	this_thread::sleep_for(1s);
	X.pop_front();
	cout << __func__ << " Give signal\n";
	p.set_value();
}

void
read_list()
{
	for (;;)
	{
		cout << "R: ";
		for (ptr_list<IMBUSdev>::iterator it = mDevList; it != mDevList.end(); it++) {
			cout << "Current ptr use count " << (*it).use_count() << endl;
			cout << static_cast<int>((**it).modaddr) << " ";
		}
		cout << endl;
		this_thread::sleep_for(200ms);
	}
}

void
append_dev_to_list()
{
	uint8_t i {1};

	for (;;)
	{
		if (mutex_terminate.try_lock()) {
			cout << "Receive signal to terminate " << __func__ << endl;;
			mDevList.clear();
			return;
		}

		mDevList.push(new IMBUSdev {i++});
		cout << "Append:" << static_cast<int>(i) << endl;
		this_thread::sleep_for(1s);
	}
}

void
remove_dev_from_list()
{
	for (;;)
	{
		// mDevList.remove(reinterpret_cast<void*>(i++));
		// cout << "Remove:" << i << endl;
		cout << "Pop front\n";
		mDevList.pop_front();
		this_thread::sleep_for(800ms);
	}
}

void
read_use_count()
{
	for (;!mutex_signal.try_lock(); this_thread::sleep_for(1s));
	ptr_list<IMBUSdev>::iterator it {mDevList};

	int i {0};
	for (ptr_list<IMBUSdev>::ptr<IMBUSdev> r = *it;; this_thread::sleep_for(1s)) {
		cout << __func__ << ": current ptr use count " << r.use_count() << "=" << (*it).use_count() << endl;
		if (i++ == 6)
			break;
	}

	cout << __func__ << ": exit!\n";
	mutex_signal.unlock();
}

void
send_signal()
{
	while (mDevList.list.empty())
		this_thread::sleep_for(1s);

	for (;;this_thread::sleep_for(1s))
	{
		std::lock_guard<std::mutex> lck {mutex_signal};
		cout << __func__ << " notify_one\n";
		mDevList.cv.notify_one();
	}
}

int main()
{
	std::forward_list<int> l = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
 
	//    l.erase( l.begin() ); // ERROR: No function erase
 
	l.erase_after( l.before_begin() ); // Removes first element
 
	for( auto n : l ) std::cout << n << " ";
	std::cout << '\n';
 
	auto fi = std::next( l.begin() );
	cout << "next(l.begin()) " <<  *fi << endl;
	auto la = std::next( fi, 3 );
	cout << "next(" << *fi << ",3) " << *la << endl;
	l.erase_after( fi, la );
 
	for (auto n : l)
		std::cout << n << " ";
	std::cout << '\n';

// -----------------------------------------
	// Test barrier by using promise
	for (int i = 1; i <= 10; i++)
		X.push_front(i);

	for (auto n : X)
		std::cout << n << " ";
	cout << endl;

	std::forward_list<int>::iterator x_it {X.begin()};
	for (int i = 0; i < 12 && x_it != X.end(); i++) {
		cout << i << "\t" << *x_it << endl;
		cout << i << "\t" << *x_it++ << endl << endl;
	}

	std::forward_list<int>::iterator it {X.begin()};
	promise<void> barrier;
	future<void> barrier_future {barrier.get_future()};
	thread t {do_work, std::move(barrier)};
	barrier_future.wait();
	it = X.begin();
	cout << *it << endl;
	it++;
	cout << *it << endl;

// -----------------------------------------
	mutex_signal.lock();
	IMBUSdev d1 {1};
	// IMBUSdev d2 {2};
	mDevList.push(&d1);
	// mDevList.push(&d2);

	// thread R {read_list};
	// thread A {append_dev_to_list};
	// thread D {remove_dev_from_list};
	thread B {read_use_count};
	// thread X {send_signal};

	for (int i = 0; i < 100; i++) {
		if (i == 2)
			mutex_signal.unlock();

		if (mDevList.list.empty())
			cout << __func__ << ": no device in list\n";
		else
			cout << __func__ << ": dev ptr list first element-" << mDevList.list.front().use_count() << endl;

		this_thread::sleep_for(900ms);

		if (i > 3 && !mDevList.list.empty()) {
			// mDevList.pop_front();
			mutex_signal.unlock();
			mDevList.remove(&d1);
			// if (X.joinable())
			//	X.join();
		}
	}
}
