#include <string>
#include <iostream>
using namespace std;
int
main(int argc, char *argv[])
{
        string s {"hello"};
        cout << s << s.length();
        s = "OH";
        cout << s << s.length();
        return 0;
}
