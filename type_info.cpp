#include <iostream>
#include <unordered_map>
#include <typeindex>

struct A {
        A(int j)
                : i {j} 
                {
                        map.insert({i, this});
                }
        int i;
        static std::unordered_map<int,A*> map;
};

std::unordered_map<int,A*> A::map;

struct AA {
        AA()
                : a {1}, b{2}
                {}
        A a;
        A b;
};


AA aa;

int
main(int argc, char *argv[])
{
        std::cout << typeid(A::map[1]).name() << std::endl;
        
        return 0;
}
