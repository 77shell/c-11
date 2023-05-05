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
		std::weak_ptr<S> wekptr {p1};
	}
	cout << "No S dtor invoke\n";
}
