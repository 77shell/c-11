
extern "C" {
#include <stdio.h>
#include <sys/stat.h>
}

#include <fstream>
#include <iostream>

using namespace std;

long get_filesize(const char *path)
{
        struct stat stat_buf;
        int rc = stat(path, &stat_buf);
        return rc == 0 ? stat_buf.st_size : -1;
}


int main(int argc, char *argv[])
{
        const char *file {"/home/max/src/csu.linux/genset/unit-test/access_log.db"};
        fprintf(stderr, "C: File size: %ld\n", get_filesize(file));
        
        ifstream in {file, istream::ate | ifstream::binary};
        if(in) {
                long s {in.tellg()};
                cout << "C++: File size: " << s << endl;
        }
        else {
                cout << "Open " << file << " fails~" << endl;
        }

        if(-1)
                cout << "hello" << endl;
        else
                cout << "ohyeah" << endl;
}
