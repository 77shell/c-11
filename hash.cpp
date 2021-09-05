#include <functional>
#include <string>
#include <iostream>
using namespace std;
int main(int argc, char *argv[])
{
        for(int i=0; i<10; i++)
        {
                cout << i << " hash<int> : " << hash<int>()(i) << endl;
        }
        cout << "hello's hash<string> : " << hash<string>()(string {"hello"}) << endl;
}
