#include <iostream>
#include <cassert>
#include <cstring>
#include <string>
#include <chrono>
#include <memory>
#include <vector>
#include <unistd.h>
using namespace std;

std::string
gen_timestamp()
{
	using namespace std::chrono;
	system_clock current;
	std::time_t t = current.to_time_t(current.now());
	char str_buf[32];
	std::strftime(str_buf, 32, "%F %T", std::localtime(&t));
	return std::string {str_buf};
}

struct Register_t {
	Register_t() = default;
	~Register_t() {
		cout << __func__ << endl;
	}
	std::string name;
};

struct RegisterTable_t {
	RegisterTable_t() {
		cout << __func__ << endl;
	}
	~RegisterTable_t() {
		cout << __func__ << endl;
	}
	std::vector<Register_t> vec;
};

int main(int argc, char *argv[])
{
        using namespace std;
	{
		unique_ptr<RegisterTable_t[]> {new RegisterTable_t[3]};
	}
	sleep(3);
        return 0;
}
