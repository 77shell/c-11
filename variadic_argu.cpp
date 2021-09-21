#include <chrono>
#include <stdio.h>
#include <iostream>
#include <stdarg.h>

struct debugmsg {
	debugmsg(const char *title, const char *msg...);
	debugmsg(const char *msg);
};

char*
timestamp(char *strbuf, size_t bufsize)
{
        using namespace std::chrono;
        system_clock current;
        std::time_t utc = current.to_time_t(current.now());
        std::strftime(strbuf, bufsize, "%F %T", std::localtime(&utc));
	return strbuf;
}

debugmsg::debugmsg(const char *title, const char *msg...)
{
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	// Variadic argument string buffer
	constexpr size_t va_str_len {64};
	char msg_str[va_str_len];

	va_list args;
	va_start(args, msg);
	vsnprintf(msg_str, va_str_len, msg, args);
	va_end(args);

	constexpr size_t t_len {24};
	char t_str[t_len];

	fprintf(stderr, "%s> %s -- %s\n", timestamp(t_str, t_len), title, msg_str);
}

debugmsg::debugmsg(const char *msg)
{
	constexpr size_t t_len {24};
	char t_str[t_len];
	fprintf(stderr, "%s> %s\n", timestamp(t_str, t_len), msg);
}

#define __MESG_STATE_MACH

#ifdef __MESG_STATE_MACH
#define MESG debugmsg
#else
#define MESG
#endif

int
main(int argc, char *argv[])
{
	MESG(__func__, "hello %d", 10);
	MESG("hello %d\n");
}
