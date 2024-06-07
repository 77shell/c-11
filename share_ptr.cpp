#include <memory>
#include <iostream>

using namespace std;

struct S {
	S() {
		cout << __func__ << i << endl;
	}

	S(int x)
		: i {x}
		{
			cout << __func__ << i << endl;
		}

	~S() {
		cout << __func__ << i << endl;
	}

	int i {0b1111};
};

int
main()
{
	S *p {new S};
	S *p1 {new S {1111}};
	{
		std::shared_ptr<S> shptr {p};
		std::weak_ptr<S> wekptr {shptr};
		std::weak_ptr<S> wekptr2 {};

		if (std::shared_ptr<S> ptr {}; ptr && ptr->i > 0) {
			std::cout << "A. Non-empty shared_ptr\n";
		}
		else {
			std::cout << "empty shared_ptr\n";
		}

		if (std::shared_ptr<S> ptr {wekptr.lock()})
			std::cout << "B. Non-empty shared_ptr\n";
	}
	cout << "No S dtor invoke\n";
}
