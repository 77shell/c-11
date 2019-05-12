
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <vector>
#include <iostream>

using namespace std;

class No_copy {
public:
        No_copy(const No_copy&) = delete;
};

class My_error {
public:
        My_error() {}
};


void h()
{
        string s = "not";
        throw My_error{};
        string s2 = "at all";
}


void g()
{
        string s = "excess";
        {
                string s = "or";
                h();
        }
}


void f()
{
        string name {"Byon"};
        try {
                string s = "in";
                g();
        }
        catch(My_error) {
        }
}


int main(int argc, char *argv[])
{
        f();
        return 0;
}
