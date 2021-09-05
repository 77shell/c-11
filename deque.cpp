#include <iostream>
#include <deque>

int main(int argc, char *argv[])
{
        std::deque<unsigned char> q;
        unsigned char d {0x31};
        for(ssize_t i=0; i<50; i++, d++)
                q.push_back(d);
        fprintf(stdout, "%s\n", q.get_allocator());
}
