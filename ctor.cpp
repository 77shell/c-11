#include <memory>
#include <iostream>
#include <chrono>
#include <thread>
#include <memory.h>
#include <mutex>

using namespace std;
using namespace chrono_literals;

class X {
public:
        X(const char *str)
                : s {strdup(str)}
                {
                        cout << "X(const char *str) constructor " << s << endl;
                }

        //X(const X&) = delete;
        //X& operator=(const X &) = delete;
        
        virtual ~X() {
                cout << "dtor ~X() " << reinterpret_cast<long>(s) << "\t" << s << endl;
                free(s);
        }


        X(const X &x)
                {
                        if(x.s)
                                s = strdup(x.s);
                        cout << "copy constructor : X(const X &)" << endl;
                }
#if 1
        X& operator=(const X &x) {
                strcpy(s, x.s);
                cout << "copy assignment=(const X&)" << endl;
                return *this;
        }
#endif

        char *s;
};

class Y : public X {
public:
	Y(const char *name)
		: X {name}
		{}

	virtual ~Y() {
		cout << "dtor " << __func__ << endl;
	}
};

class MyInt {
        int m;
 public:
         MyInt(int i)
                 : m {i}
                 {}


        MyInt(MyInt &i)
                {
                        i.m++;
                        m = i.m;
                }
};

std::mutex mutex_terminate;

void
task(std::shared_ptr<X> p, string label)
{
	std::unique_ptr<X> y {new Y {label.c_str()}};
	for (;;)
	{
		if (mutex_terminate.try_lock()) {
			cout << "Receive signal to terminate task\n";
			return;
		}
		cout << label << "-" << __func__ << " " << p.use_count() << endl;
		this_thread::sleep_for(1s);
	}
}

shared_ptr<X> XX;

shared_ptr<X>
get_x()
{
	return XX;
}

int main(int argc, char *argv[])
        try {
                X x1 {"whatever"};
                //X x2 {x1};
                //X x3 = x1;
                //x2 = x3;

                X x4 {"youdontknow"};
                // x4 = x1;

		X x5 = x1; // invoke copy ctor
		X x6 {x4}; // invoke copy ctor

		cout << "hello ------------------- \n";


                MyInt a {1};
                MyInt b {a};

		{
			int i {1};
			// std::unique_ptr<X> x5 {new X {"UNIQUE_PTR"}};
			XX.reset(new X {"SHARED_PTR"});
			mutex_terminate.lock();
			// thread t1 {&task, std::shared_ptr<X>(x5.get()), string {"t1"}};
			thread t2 {&task, std::shared_ptr<X>(get_x()), string {"t2"}};
			thread t3 {&task, std::shared_ptr<X>(get_x()), string {"t3"}};
			// pthread_t pid {t1.native_handle()};

			for (shared_ptr<X> x = get_x(); i < 9; i++) {
				this_thread::sleep_for(1s);
				cout << __func__ << " use count " << x.use_count() << endl;
				if (i == 2) {
					mutex_terminate.unlock();
				}
				if (i == 3) {
					mutex_terminate.unlock();
				}
				// if (i == 4) {
				// 	mutex_terminate.unlock();
				// }
				// cout << __func__ << " " << i << endl;
			}

			// if (t1.joinable())
			// 	t1.join();

			if (t2.joinable())
				t2.join();

			if (t3.joinable())
				t3.join();

			cout << __func__ << " use count " << XX.use_count() << endl;
			XX.reset();
		}

		for (;;)
		{
			cout << "Use count " << get_x().use_count() << endl;
			this_thread::sleep_for(1s);
		}
        }
        catch(...) {
        }
