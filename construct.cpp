
#include <initializer_list>
#include <iostream>

using namespace std;

struct S { int a, b; };
struct SS { double a, b; };

void f(S s)
{
        cout << "f(), S.a " << s.a << endl;
}


void g(S s)
{
        cout << "s(S), S.a " << s.a << endl;
}


void g(SS ss)
{
        cout << "g(SS), SS.a " << ss.a << endl;
}


int
main(int argc, char *argv[])
{
        f({1, 2});
        
        //g({1, 2});
        g(S{2, 3});
        g(SS{2, 3});
        
        exit(EXIT_SUCCESS);
}
