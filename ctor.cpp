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

        X(const X&) = delete;
        X& operator=(const X &) = delete;
        
        virtual ~X() {
                cout << "destructor ~X() " << reinterpret_cast<long>(s) << "\t" << s << endl;
                free(s);
        }

#if 0
        X(const X &x)
                {
                        if(x.s)
                                s = strdup(x.s);
                        cout << "copy constructor : X(const X &)" << endl;
                }

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

std::mutex mutex_terminal;

void
task(std::shared_ptr<X> p)
{
	std::unique_ptr<X> y {new Y {"Hello"}};
	for (;;)
	{
		if (mutex_terminal.try_lock()) {
			cout << "Receive signal to terminate task\n";
			return;
		}
		cout << __func__ << " " << p.use_count() << endl;
		this_thread::sleep_for(1s);
	}
}

int main(int argc, char *argv[])
        try {
                X x1 {"whatever"};
                //X x2 {x1};
                //X x3 = x1;
                //x2 = x3;

                X x4 {"youdontknow"};
                //x4 = x1;

                MyInt a {1};
                MyInt b {a};

		int i {1};
		std::unique_ptr<X> x5 {new X {"UNIQUE_PTR"}};
		mutex_terminal.lock();
		thread t {&task, std::shared_ptr<X>(x5.get())};
		pthread_t pid {t.native_handle()};

		for (;;) {
			i++;
			this_thread::sleep_for(1s);
			if (i == 4) {
				mutex_terminal.unlock();
				t.join();
			}
			cout << __func__ << " " << i << endl;
		}
        }
        catch(...) {
        }
