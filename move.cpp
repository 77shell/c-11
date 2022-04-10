#include <string>
#include <iostream>

using namespace std;

void
consume_str(string s)
{
	string tmp {std::move(s)};
	cout << __func__ << s << endl;
	cout << __func__ << tmp << endl;
}

int
main()
{
	string s1 {"hello"};
	consume_str(s1);
	cout << __func__ << s1 << endl;
	return 0;
}
