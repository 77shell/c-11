#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

int
main(int argc, char *argv[])
{
	system_clock current;
	std::time_t t = current.to_time_t(current.now());
	const size_t str_buf_size {40};
	char str_buf[str_buf_size];
	std::strftime(str_buf, str_buf_size, "_%F_%H%M", std::localtime(&t));
	cout << str_buf << endl;
}
