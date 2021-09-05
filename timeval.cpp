#include <stdio.h>
#include <sys/time.h>
#include <iostream>

void
timeval_normalize(struct timeval &tv, time_t sec, suseconds_t us)
{
        constexpr suseconds_t max_tv_usec {999999};
        if(sec < 0) {
                sec = 0;
                us = 0;
                std::cerr << __func__ << " negative time input (sec)\n";
        }
        else {
                if(us < 0) {
                        sec = 0;
                        us = 0;
                        std::cerr << __func__ << " negative time input (usec)\n";
                }
                else if(us > max_tv_usec) {
                        constexpr suseconds_t _1sec_in_tv_usec {1000000};
                        const time_t s {us / _1sec_in_tv_usec};
                        sec += s;
                        us = us - s * _1sec_in_tv_usec;
                }
        }
        tv.tv_sec = sec;
        tv.tv_usec = us;
}

void
print_timeval(struct timeval &t)
{
        printf("tv_sec : %ld\n", t.tv_sec);
        printf("tv_usec: %d\n", t.tv_usec);
}

void
test_timeval_normailize(time_t s, suseconds_t us)
{
        printf("Non-normalized sec.us: %ld.%d\n", s, us);
        struct timeval t;
        timeval_normalize(t, s, us);
        print_timeval(t);
}

int main(int argc, char *argv[])
{
        test_timeval_normailize(-1, 999);
        test_timeval_normailize(-1, -1);
        test_timeval_normailize(0, 0);
        test_timeval_normailize(0, 999999);
        test_timeval_normailize(0, 1000000);
        test_timeval_normailize(0, 4000000);
        test_timeval_normailize(0, 1000000000);
        test_timeval_normailize(0, 2000);

        test_timeval_normailize(-1, 999);
        test_timeval_normailize(-1, -1);
        test_timeval_normailize(4, 0);
        test_timeval_normailize(3, 999999);
        test_timeval_normailize(6, 1000000);
        test_timeval_normailize(7, 4000000);
        test_timeval_normailize(8, 1000000000);
        test_timeval_normailize(1000, 2000);

        return 0;
}
