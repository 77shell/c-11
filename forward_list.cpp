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
		void operator()(T *_p) const {
			std::cout << "Call delete from function object " << _p << endl;
		}
	};

	struct ptr : public std::shared_ptr<T> {
		ptr(T *src, ptr_deleter d, ptr_list *p)
			: std::shared_ptr<T> {src, d}, ptrlist {p}
			{}

		ptr(ptr_list *p)
			: ptrlist {p}
			{
				cout << __func__ << p << endl;
			}

		~ptr() {
			cout << __func__ << endl;
			if (ptrlist) {
				cout << "### " << __func__ << ": p use count " << std::shared_ptr<T>::use_count() << endl;
				if (std::shared_ptr<T>::use_count())
					ptrlist->cv.notify_one();
			}
		}

		void reset() {
			cout << __func__ << endl;
			std::shared_ptr<T>::reset();
			if (ptrlist)
				ptrlist->cv.notify_one();
		}

		ptr_list *ptrlist;
	};

	void push(T *_p) {
		std::lock_guard<std::mutex> lck {list_lock};
		if (!list.empty()) {
			for (auto &p : list)
				if (p.get() == _p)
					return;
		}

		list.push_front(ptr {_p, ptr_deleter {}, this});
		listchanged_flag.clear();
	}

	void remove(T *_p) {
		std::unique_lock<std::mutex> lck {list_lock};
		if (list.empty())
			return;

		cout << __func__ << ": start ---------------------\n";
		list.remove_if(
			[&](ptr &p) {
				if (p.get() == _p) {
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
		cout << __func__ << ": end ---------------------- \n";
	}

	void remove(ptr &&_p) {
		std::unique_lock<std::mutex> lck {list_lock};
		cout << __func__ << ": start ---------------------\n";
		cout << __func__ << " use count: " << _p.use_count() << endl;
		if (list.empty())
			return;

		cout << __func__ << endl;
		list.remove_if(
			[&](ptr &p) {
				if (p.get() == _p.get()) {
					cv.wait(lck, [&]() {
							     cout << "$$$ " << __func__ << ": p use count " << p.use_count() << endl;
							     return p.use_count() <= 1;
						     }
						);
					_p.ptrlist = nullptr;
					return true;
				}
				return false;
			}
			);
		listchanged_flag.clear();
		cout << __func__ << ": end ---------------------- \n";
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

	size_t count() {
		std::lock_guard<std::mutex> lck {list_lock};
		return std::distance(list.begin(), list.end());
	}

	struct iterator {
		iterator(ptr_list &v)
			: _ptrlist {v}
			{
				std::lock_guard<std::mutex> lck {_ptrlist.list_lock};
				list_iterator = _ptrlist.list.begin();
			}

		bool operator!=(const typename std::forward_list<ptr>::iterator &rhs) {
			std::lock_guard<std::mutex> lck {_ptrlist.list_lock};
			return list_iterator != rhs;
		}

		bool operator==(const typename std::forward_list<ptr>::iterator &rhs) {
			std::lock_guard<std::mutex> lck {_ptrlist.list_lock};
			return list_iterator == rhs;
		}

		ptr operator*() {
			std::lock_guard<std::mutex> lck {_ptrlist.list_lock};
			return *list_iterator;
		}

		ptr operator++(int) { // Post-increment, iterator++
			std::lock_guard<std::mutex> lck {_ptrlist.list_lock};
			// return list_iterator == _ptrlist.list.end()
			// 	? nullptr
			// 	: *list_iterator++;
			if (list_iterator == _ptrlist.list.end())
			{
				cout << __func__ << " list_iterator reach to the end\n";
				return nullptr;
			}
			else
			{
				cout << __func__ << " list_iterator doesn't reach to the end\n";
				return *list_iterator++;
			}
		}

	private:
		ptr_list &_ptrlist;
		typename std::forward_list<ptr>::iterator list_iterator;
	};

	typename std::forward_list<ptr>::iterator begin() {
		std::lock_guard<std::mutex> lck {list_lock};
		return list.begin();
	}

	typename std::forward_list<ptr>::iterator end() {
		std::lock_guard<std::mutex> lck {list_lock};
		return list.end();
	}

	std::atomic_flag listchanged_flag;
	std::mutex list_lock;
	std::condition_variable cv;
	std::forward_list<ptr> list;
};

ptr_list<IMBUSdev> mDevList;

std::forward_list<int> X;
std::mutex mutex_terminate;
std::mutex mutex_signal_a;
std::mutex mutex_signal_b;

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

IMBUSdev d1 {1};
IMBUSdev d2 {2};
IMBUSdev d3 {3};
IMBUSdev d4 {4};
void watch_add();
void remove_dev();

int
main()
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
	if (t.joinable())
		t.join();

// Test add
// -----------------------------------------
	mutex_signal_a.lock();
	mutex_signal_b.lock();

	mDevList.push(&d1);
	thread task {watch_add};
	cout << __func__ << " locking signal-a\n";
	mutex_signal_a.lock(); // wait task watch_add
	cout << __func__ << " signal-a unlocked()\n";
	mDevList.push(&d2);
	mDevList.push(&d3);
	mDevList.push(&d4);
	cout << __func__ << "List cout: " << mDevList.count() << endl;
	cout << __func__ << " unlocking signal-b\n";
	mutex_signal_b.unlock();

	if (task.joinable())
		task.join();

// Test remove
// -----------------------------------------
	cout << "\n\n-----------Test remove() ---------------\n";
	{
		ptr_list<IMBUSdev>::ptr ptr_begin {*mDevList.begin()};
		auto p = std::async(std::launch::async, remove_dev);
		for (int i = 0; i < 5; i++, this_thread::sleep_for(1s))
			cout << ".\n";
		ptr_begin.reset();
	}

	if (task.joinable())
		task.join();

	for(;;)
	{
		cout << "hello\n";
		this_thread::sleep_for(2s);
		break;
	}
}

void
watch_add()
{
	ptr_list<IMBUSdev>::iterator it {mDevList};
	cout << __func__ << ": modaddr " << static_cast<int>((*it)->modaddr) << endl;
	it++;
	cout << __func__ << " unlocking signal-a\n";
	mutex_signal_a.unlock();
	mutex_signal_b.lock();
	cout << __func__ << " signal-b unlocked\n";

	if (it == mDevList.end()) {
		cout << __func__ << " reach to end of list\n";
	}
	else {
		cout << __func__ << " iterator changed , (*it)->modaddr " << static_cast<int>((*it)->modaddr) << endl;
	}
	
}

void
remove_dev()
{
	std::forward_list<ptr_list<IMBUSdev>::ptr>::iterator it {mDevList.begin()};
	cout << __func__ << ": modaddr " << static_cast<int>((*it)->modaddr) << endl;
	mDevList.remove(std::move(*it));
	it = mDevList.begin();
	for (int i = 0; i < 3; i++) {
		this_thread::sleep_for(1s);
		cout << __func__ << ": modaddr " << static_cast<int>((*it++)->modaddr) << endl;
	}

}
