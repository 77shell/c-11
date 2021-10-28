#include <ctime>
#include <chrono>
#include <string>
int
main(int argc, char *argv[])
{
        constexpr size_t STR_DATETIME_LEN {32};
        using namespace std::chrono;
        system_clock current;
        std::time_t utc = current.to_time_t(current.now());
        char str_buf[STR_DATETIME_LEN];
        std::strftime(str_buf, STR_DATETIME_LEN, "%F %T", std::localtime(&utc));
	fprintf(stderr, "%s", str_buf);
}
