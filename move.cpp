#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
using namespace std;

void
revise(string &&s)
{
	cout << __func__ << s << s.size() << endl;
	string t {std::move(s)};
	cout << __func__ << s << s.size() << endl;
	t = "world";
}

void
revise(vector<int> &&v)
{
	cout << __func__ << " vector size: " << v.size() << endl;
	vector<int> i {std::move(v)};
	cout << __func__ << " vector size: " << i.size() << endl;
}

struct S {
	S()
		: label {"default-name"}, id {}
		{
			cout << __func__ << " default ctor\n";
		}

	S(std::string name)
		: label {name}
		{
			cout << __func__ << " ctor-1 label " << label << std::endl;
		}

	S(S &&rval)
		: label {std::move(rval.label)}
		{
			cout << __func__ << " move ctor label " << label << std::endl;
		}

	~S()
		{
			cout << __func__ <<  " dtor label " << label << std::endl;
		}
	std::string label;
	int id;
};

int
main()
{
	string s {"hello"};
	cout << __func__ << s << s.size() << endl;;

	revise(std::move(s));
	cout << __func__ << s << s.size() << endl;

	vector<int> v (10, 0xaabb);
	cout << __func__ << " vector size: " << v.size() << endl;
	revise(std::move(v));
	cout << __func__ << " vector size: " << v.size() << endl;

	std::vector<S> vec_s;
	vec_s.emplace_back("hello");
	int x {};
	cout << "X " << x << endl;
	sleep(1);
	exit(0);
}


void
consume_str(string s)
{
	string tmp {std::move(s)};
	cout << __func__ << s << endl;
	cout << __func__ << tmp << endl;
}

int
_main()
{
	string s1 {"hello"};
	consume_str(s1);
	cout << __func__ << s1 << endl;
	return 0;
}
