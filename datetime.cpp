
#include <iostream>
#include <chrono>
#include <ctime>
#include <array>
#include <iomanip>
#include <linux/version.h>

using namespace std;

enum class datetime : std::size_t {
	year = 0,
	month = 1,
	mday = 2,
	hour = 3,
	minute = 4,
	second = 5
};
constexpr std::size_t datetime_len {6};
using datetime_t = std::array<uint8_t, datetime_len>;

datetime_t convert_datetime(std::time_t utc_sec)
{
	std::tm tm {*std::localtime(&utc_sec)};
	datetime_t r {};
	int y {tm.tm_year + 1900 - 2000}; // since 2000
	if (y < 0)
		y = 0;

	r[static_cast<std::size_t>(datetime::year)]   = static_cast<uint8_t>(y);
	r[static_cast<std::size_t>(datetime::month)]  = static_cast<uint8_t>(tm.tm_mon);
	r[static_cast<std::size_t>(datetime::mday)]   = static_cast<uint8_t>(tm.tm_mday);
	r[static_cast<std::size_t>(datetime::hour)]   = static_cast<uint8_t>(tm.tm_hour);
	r[static_cast<std::size_t>(datetime::minute)] = static_cast<uint8_t>(tm.tm_min);
	r[static_cast<std::size_t>(datetime::second)] = static_cast<uint8_t>(tm.tm_sec);
	return r;
}

std::time_t convert_time(datetime_t datetime)
{
	using namespace std::chrono;
	system_clock current;
	std::time_t sec = current.to_time_t(current.now());
	std::tm local = *std::localtime(&sec);
	int y = datetime[static_cast<std::size_t>(datetime::year)] + 2000; // since 2000
	local.tm_year = y - 1900; // since 1900
	local.tm_mon  = datetime[static_cast<std::size_t>(datetime::month)];
	local.tm_mday = datetime[static_cast<std::size_t>(datetime::mday)];
	local.tm_hour = datetime[static_cast<std::size_t>(datetime::hour)];
	local.tm_min  = datetime[static_cast<std::size_t>(datetime::minute)];
	local.tm_sec  = datetime[static_cast<std::size_t>(datetime::second)];
	return std::mktime(&local);
}

// namespace std {
//     time_t timegm(tm* _Tm) 
//     {
//         auto t = mktime(_Tm);
//         return t + (mktime(localtime(&t)) - mktime(gmtime(&t)));
//     }
// }

static void
print_date_time(std::tm &datetime)
{
        cout << "Year: " << datetime.tm_year << endl;
        cout << "Month: " << datetime.tm_mon + 1 << endl;
        cout << "Day: " << datetime.tm_mday << endl;
        cout << "Hour: " << datetime.tm_hour << endl;
        cout << "Min: " << datetime.tm_min << endl;
        cout << "Sec: " << datetime.tm_sec << endl;
}

const char *getBuild() { //Get current architecture, detectx nearly every architecture. Coded by Freak
#if defined(__x86_64__) || defined(_M_X64)
	return "x86_64";
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
	return "x86_32";
#elif defined(__ARM_ARCH_2__)
	return "ARM2";
#elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
	return "ARM3";
#elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
	return "ARM4T";
#elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
	return "ARM5"
#elif defined(__ARM_ARCH_6T2_) || defined(__ARM_ARCH_6T2_)
		return "ARM6T2";
#elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
	return "ARM6";
#elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
	return "ARM7";
#elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
	return "ARM7A";
#elif defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
	return "ARM7R";
#elif defined(__ARM_ARCH_7M__)
	return "ARM7M";
#elif defined(__ARM_ARCH_7S__)
	return "ARM7S";
#elif defined(__aarch64__) || defined(_M_ARM64)
	return "ARM64";
#elif defined(mips) || defined(__mips__) || defined(__mips)
	return "MIPS";
#elif defined(__sh__)
	return "SUPERH";
#elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
	return "POWERPC";
#elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
	return "POWERPC64";
#elif defined(__sparc__) || defined(__sparc)
	return "SPARC";
#elif defined(__m68k__)
	return "M68K";
#else
	return "UNKNOWN";
#endif
}

int main(int argc, char *argv[])
{
#if defined(__x86_64__)
/* 64 bit detected */
	cout << "x86_64 machine\tsizeof(std::time_t) : " << sizeof(std::time_t) << '\n';
#else
	cout << "unknown machine\tsizeof(std::time_t) : " << sizeof(std::time_t) << '\n';
#endif

        //std::time_t since_epoc_sec {std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
        std::time_t since_epoc_sec {std::time(nullptr)};
        cout << "Since epoc (seconds): " << since_epoc_sec << '\t' << std::hex << since_epoc_sec << "h\n" << std::dec;

        print_date_time(*std::gmtime(&since_epoc_sec));
        cout << "Local date/time\n";
        print_date_time(*std::localtime(&since_epoc_sec));

	// Inverse gmtime(time_t) -> tm
	// timegm(tm*) -> time_t
	{
		cout << "\nUTC 2000-1-1 00:00:00\n";
		std::tm utc_tm {};
		utc_tm.tm_year = 100; // since 1900
		utc_tm.tm_mon = 1;
		utc_tm.tm_mday = 1;
		utc_tm.tm_hour = 0;
		utc_tm.tm_min = 0;
		utc_tm.tm_sec = 0;
		std::time_t t0 {timegm(&utc_tm)};
		cout << "UTC second: " << t0 << endl;
		print_date_time(*std::gmtime(&t0));
		cout << "Local date/time\n";
		print_date_time(*std::localtime(&t0));
	}

	{
		cout << "\nUTC 2099-12-31 23:59:59\n";
		std::tm utc_tm {};
		utc_tm.tm_year = 199; // since 1900
		utc_tm.tm_mon = 11;
		utc_tm.tm_mday = 31;
		utc_tm.tm_hour = 23;
		utc_tm.tm_min = 59;
		utc_tm.tm_sec = 59;
		std::time_t t0 {timegm(&utc_tm)};
		cout << "UTC second: " << t0 << endl;
		print_date_time(*std::gmtime(&t0));
		cout << "Local date/time\n";
		print_date_time(*std::localtime(&t0));
	}

 
	{
		cout << endl;
		setenv("TZ", "/usr/share/zoneinfo/Europe/London", 1); // POSIX-specific
 
		std::tm tm{};  // get_time does not set all fields hence {}
		tm.tm_year = 2199-1900; // 2199
		tm.tm_mon = 12-1; // July
		tm.tm_mday = 31; // 31st
		tm.tm_hour = 23;
		tm.tm_min = 59;
		tm.tm_isdst = 1; // Daylight saving in London
		std::time_t t = std::mktime(&tm); 
 
		std::cout << "UTC:   " << std::put_time(std::gmtime(&t), "%c %Z") << '\n';
		std::cout << "local: " << std::put_time(std::localtime(&t), "%c %Z") << '\n';
	}

	{
		cout << endl;
		using namespace std::chrono;
		system_clock current;
		uint64_t utc_sec = current.to_time_t(current.now());
		cout << "utc_sec " << utc_sec << endl;
		cout << getBuild() << endl;
	}


	{
		uint64_t utc_sec {UINT64_MAX};
		cout << "utc_sec\t" << std::hex << utc_sec << endl;
		if (sizeof(std::time_t) == 4) {
			if (utc_sec > std::numeric_limits<std::time_t>::max()) {
				cout << "std::numeric_limits<std::time_t>::max() \t" << std::numeric_limits<std::time_t>::max() << endl;
				utc_sec = std::numeric_limits<std::time_t>::max();
			}
		}

		std::time_t usec = static_cast<std::time_t>(utc_sec);
		std::tm *tm {std::gmtime(&usec)};
		if (!tm) {
			std::cerr << __func__ << " cannot convert utc_sec " << usec << '\n';
		}
		std::cout << "UTC:   " << std::put_time(std::gmtime(&usec), "%c %Z") << '\n';
	}
	
	{
		std::time_t usec = std::numeric_limits<std::time_t>::max();
		std::tm *tm {std::gmtime(&usec)};
		if (!tm) {
			std::cerr << __func__ << " cannot convert utc_sec " << usec << '\n';
		}
		// UTC:   Tue Jan 19 03:14:07 2038 GMT
		std::cout << "UTC:   " << std::put_time(std::gmtime(&usec), "%c %Z") << '\n'; // 7fff'ffffh

		// UTC:   Thu Jan  1 00:00:00 1970 GMT
		usec = 0;
		std::cout << "UTC:   " << std::put_time(std::gmtime(&usec), "%c %Z") << '\n';

		// UTC:   Wed Dec 31 23:59:59 1969 GMT
		usec = 0xffffffff;
		std::cout << "UTC:   " << std::put_time(std::gmtime(&usec), "%c %Z") << '\n';

		// UTC:   Wed Dec 31 23:59:58 1969 GMT
		usec = 0xfffffffe;
		std::cout << "UTC:   " << std::put_time(std::gmtime(&usec), "%c %Z") << '\n';

		// UTC:   Fri Dec 13 20:45:52 1901 GMT
		usec = 0x80000000;
		std::cout << "UTC:   " << std::put_time(std::gmtime(&usec), "%c %Z") << '\n';

		// UTC:   Fri Dec 13 20:45:53 1901 GMT
		usec = 0x80000001;
		std::cout << "UTC:   " << std::put_time(std::gmtime(&usec), "%c %Z") << '\n';
	}
}
