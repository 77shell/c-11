#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

struct A {
	A(const char *s)
		: str {strdup(s)}
		{}

	~A() {
		free(str);
	}

	A(const A &a)
		: str {strdup(a.str)}
		{
			cout << __func__ << "'s copy ctor\n";
		}

	A& operator=(const A &a) {
		cout << __func__ << "'s assignment operator\n";
		free(str);
		str = strdup(a.str);
		return *this;
	}

private:
	char *str;
};

int
main(int argc, char *argv[])
{
	A a1 {"hello"};

	// If no copy constructor then causes double free()
	A a2 {a1};

	// Invoke copy construtor
	A a3 = a1;

	a3 = a1;
}
