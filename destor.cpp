
#include <iostream>

using namespace std;

class share {
public:
	share(int i)
		: m {i}
		{
			cout << __func__ << m << endl;
		}

	~share() {
		cout << __func__ << m << endl;
	}

	int m;
};

share& share_instance()
{
	static share instance(1);
	return instance;
}

class A {
public:
	A()
		: mr_share {share_instance()}, m_share {2}
		{
			cout << __func__ << endl;
		}

	~A() {
		cout << __func__ << endl;
	}

	static A& getInstance() {
		static A instance;
		return instance;
	}

	share &mr_share;
	share m_share;
};


int main(int argc, char *argv[])
{
	cout << "Enter: " << __func__ << endl;
	share_instance();
	A::getInstance();
	cout << "Exit: " << __func__ << endl;
	return 0;
}

