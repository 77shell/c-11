#include <iostream>
#include <vector>

void
bad_copy(std::vector<int> &v1)
{
        std::vector<int> v2 = v1;
        v1[0] = 2;
        v2[1] = 3;

        std::cout << "V2: ";
        for(auto i : v2)
                std::cout << i << " ";
        std::cout << std::endl;
}


int
main(int argc, char *argv[])
{
        std::vector<int> v1 {1, 2, 3, 4};

        bad_copy(v1);
        std::cout << "V1: ";
        for(auto i : v1)
                std::cout << i << " ";
        std::cout << std::endl;
        
        return 0;
}
