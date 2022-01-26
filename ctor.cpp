#include <memory.h>
#include <iostream>

using namespace std;

class X {
public:
        X(const char *str)
                : s {strdup(str)}
                {
                        cout << "X(const char *str) constructor " << s << endl;
                }

        X(const X&) = delete;
        X& operator=(const X &) = delete;
        
        ~X() {
                cout << "destructor ~X() " << reinterpret_cast<long>(s) << "\t" << s << endl;
                free(s);
        }

#if 0
        X(const X &x)
                {
                        if(x.s)
                                s = strdup(x.s);
                        cout << "copy constructor : X(const X &)" << endl;
                }

        X& operator=(const X &x) {
                strcpy(s, x.s);
                cout << "copy assignment=(const X&)" << endl;
                return *this;
        }
#endif

        char *s;
};

class MyInt {
        int m;

public:
        MyInt(int i)
                : m {i}
                {}

        MyInt(MyInt i)
                {
                        i.m++;
                        m = i.m;
                }

        
};
   

int main(int argc, char *argv[])
        try {
                X x1 {"whatever"};
                //X x2 {x1};
                //X x3 = x1;
                //x2 = x3;

                X x4 {"youdontknow"};
                //x4 = x1;

                MyInt a {1};
                MyInt b {a};
        }
        catch(...) {
        }
