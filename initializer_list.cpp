
#include <initializer_list>
#include <iostream>

using namespace std;

void f(initializer_list<unsigned int> lst)
{
        //for(int i=0; i<lst.size(); i++)
        // cout << lst[i] << endl;

        const unsigned int *p = lst.begin();
        for(unsigned int i=0; i<lst.size(); i++)
                cout << p[i] << endl;
}


void f2(initializer_list<unsigned int> lst)
{
        for(auto x : lst)
                cout << x << endl;
}


void f3()
{
        for(auto x : { 1, 2, 3, 4 })
                cout << x << endl;
}


int
main(int argc, char *argv[])
{
        initializer_list<unsigned int> lst = { 0, 1, 2, 3 };
        f(lst);
        f2(lst);
        f3();
        std::cout << "Hello world" << std::endl;
        exit(EXIT_SUCCESS);
}
