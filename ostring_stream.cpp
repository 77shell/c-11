
#include <chrono>
#include <iostream>
#include <sstream>

using u16 = unsigned short;
using u32 = unsigned long;

union datum_id_t {
        datum_id_t(u16 g, u16 i)
                : group {g}, index {i}
                {}
                        
        u32 val;
        struct {
                u16 group;
                u16 index;
        };
};


void time_stamp()
{
        using namespace std::chrono;
	system_clock current;
	std::time_t t = current.to_time_t(current.now());
	const size_t str_buf_size {40};
	char str_buf[str_buf_size];
	std::strftime(str_buf, str_buf_size, "%F %T", std::localtime(&t));
	std::string tstamp(str_buf);
        std::cout << tstamp;
}


int
main(int argc, char *argv[])
{
        short g {0xaa}, idx{0x11};
        datum_id_t i {g, idx};
        std::ostringstream s {"hello ", std::ios_base::ate};
        s.flags(std::ios_base::hex | std::ios_base::showbase);
        s << g << "-" << idx << std::endl;
        s << i.val << std::endl;
        std::cout << s.str();
        time_stamp();
        return 0;
}
